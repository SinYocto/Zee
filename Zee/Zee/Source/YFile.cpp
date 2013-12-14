#include "YFile.h"
#include "Utility.h"
#include "YString.h"

YFile* YFile::Open(const wchar_t* filePath, OPEN_MODE mode)
{
	bool isSucceed = false;

	YFile* file = NULL;
	file = New YFile();
	Assert(NULL != file);

	switch(mode)
	{
	case READ:
		_wfopen_s(&file->pFile, filePath, L"r");
		break;
	case READ_BINARY:
		_wfopen_s(&file->pFile, filePath, L"rb");
		break;
	case WRITE:
		_wfopen_s(&file->pFile, filePath, L"w");
		break;
	case APPEND:
		_wfopen_s(&file->pFile, filePath, L"a");
		break;
	default:
		Assert(false);
	}

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
	delete this;					// QUESTION:确保是new出来的?

	isSucceed = true;
Exit:
	return isSucceed;
}

wchar_t* YFile::ReadLine(wchar_t* lineContent, int maxLineSize)
{
	// TEST:每行读取读一个共13793行的文件, 花了83ms
	return fgetws(lineContent, maxLineSize, pFile);
}

// 读取由beginSpecifier和endSpecifier界定的字符串段落的每一行到blockContent中
// endSpecifier为NULL时, 则读取两个beginSpecifier之间的字符串段落
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
			lastLinePos = ftell(pFile);				// OPTIMISE:ftell很耗时
		}
	}

	isSucceed = true;
Exit:
	if(isSucceed)
		return (int)(blockContent->size());
	else
		return -1;
}

int YFile::Read(void* destBuffer, int destSize, int size, int count)
{
	return fread_s(destBuffer, destSize, size, count, pFile);
}

int YFile::WriteLine(const wchar_t* format, ...)
{
	Assert(NULL != format);

	va_list args;
	va_start(args, format);
	vfwprintf(pFile, format, args);
	va_end(args);

	fwprintf_s(pFile, L"\n");

	return true;
Exit:
	return false;
	
}

bool YFile::Exist(const wchar_t* filePath)
{
	return (_waccess(filePath, 0) == 0);
}


