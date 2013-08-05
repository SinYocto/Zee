#include "YFile.h"
#include "Utility.h"
#include "YString.h"

YFile* YFile::Open(const wchar_t* filePath, OPEN_MODE mode)
{
	bool isSucceed = false;

	YFile* file = NULL;
	file = new YFile();
	Assert(NULL != file);

	if(mode == READ)
		_wfopen_s(&file->pFile, filePath, L"r");
	else
		_wfopen_s(&file->pFile, filePath, L"w");

	Assert(NULL != file->pFile);

	isSucceed = true;
Exit:
	if(!isSucceed)
		SAFE_DELETE(file);

	return file;
}

bool YFile::reachEnd()
{
	return feof(pFile) != 0;
}

bool YFile::Close()
{
	bool isSucceed = false;

	Assert(fclose(pFile) == 0);
	delete this;					// QUESTION:ȷ����new������?

	isSucceed = true;
Exit:
	return isSucceed;
}

wchar_t* YFile::ReadLine(wchar_t* lineContent, int maxLineSize)
{
	// TEST:ÿ�ж�ȡ��һ����13793�е��ļ�, ����83ms
	return fgetws(lineContent, maxLineSize, pFile);
}

// ��ȡ��beginSpecifier��endSpecifier�綨���ַ��������ÿһ�е�blockContent��
// endSpecifierΪNULLʱ, ���ȡ����beginSpecifier֮����ַ�������
int YFile::ReadBlock(std::vector<std::wstring>* blockContent, const wchar_t* beginSpecifier, 
					 const wchar_t* endSpecifier /*= NULL*/, const wchar_t* firstLineContent /*= NULL*/)
{
	bool isSucceed = false;

	Assert(NULL != blockContent);
	Assert(NULL != beginSpecifier);

	blockContent->clear();

	bool putbackOneLine = false;
	if(endSpecifier == NULL)
	{
		endSpecifier = beginSpecifier;
		putbackOneLine = true;
	}

	bool blockStart = false;

	if(firstLineContent != NULL)
	{
		blockStart = true;

		wchar_t specifier[MAX_STR_LEN];
		YString::GetSpecifier(specifier, _countof(specifier), firstLineContent);

		Assert(YString::Compare(specifier, beginSpecifier) == 0);

		blockContent->push_back(firstLineContent);
	}

	long lastLinePos = ftell(pFile);

	wchar_t lineContent[MAX_STR_LEN];
	YString::Empty(lineContent);

	while(ReadLine(lineContent, _countof(lineContent)) != NULL)
	{
		wchar_t specifier[MAX_STR_LEN];
		YString::Empty(specifier);
		YString::Scan(lineContent, L"%s", specifier);

		if(blockStart && YString::Compare(specifier, endSpecifier) == 0)
		{
			if(putbackOneLine)
				fseek(pFile, lastLinePos, SEEK_SET);
			else
				blockContent->push_back(lineContent);

			break;
		}

		if(YString::Compare(specifier, beginSpecifier) == 0)
			blockStart = true;

		if(blockStart)
		{
			blockContent->push_back(lineContent);
			lastLinePos = ftell(pFile);				// OPTIMISE:ftell�ܺ�ʱ
		}
	}

	isSucceed = true;
Exit:
	if(isSucceed)
		return (int)(blockContent->size());
	else
		return -1;
}