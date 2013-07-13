#include "YFile.h"
#include "Utility.h"
#include "YString.h"

YFile* YFile::Open(const char* filePath, OPEN_MODE mode)
{
	bool isSucceed = false;

	YFile* file = NULL;
	file = new YFile();
	Assert(NULL != file);

	if(mode == READ)
		fopen_s(&file->pFile, filePath, "r");
	else
		fopen_s(&file->pFile, filePath, "w");

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

char* YFile::ReadLine(char* lineContent, int maxLineSize)
{
	return fgets(lineContent, maxLineSize, pFile);
}

// 读取由beginSpecifier和endSpecifier界定的字符串段落的每一行到blockContent中
// endSpecifier为NULL时, 则读取两个beginSpecifier之间的字符串段落
bool YFile::ReadBlock(std::vector<std::string>* blockContent, const char* beginSpecifier, const char* endSpecifier /*= NULL*/)
{
	bool isSucceed = false;

	Assert(NULL != blockContent);
	Assert(NULL != beginSpecifier);

	bool putbackOneLine = false;
	if(endSpecifier == NULL)
	{
		endSpecifier = beginSpecifier;
		putbackOneLine = true;
	}

	bool blockStart = false;

	fpos_t lastLinePos = 0;
	fgetpos(pFile, &lastLinePos);

	char lineContent[MAX_STR_LEN];
	YString::Empty(lineContent);

	while(ReadLine(lineContent, _countof(lineContent)) != NULL)
	{
		char specifier[MAX_STR_LEN];
		YString::Empty(specifier);
		YString::Scan(lineContent, "%s", specifier);

		if(blockStart && YString::Compare(specifier, endSpecifier) == 0)
		{
			if(putbackOneLine)
				fsetpos(pFile, &lastLinePos);
			else
				blockContent->push_back(lineContent);

			break;
		}

		if(YString::Compare(specifier, beginSpecifier) == 0)
			blockStart = true;

		if(blockStart)
		{
			blockContent->push_back(lineContent);
			fgetpos(pFile, &lastLinePos);
		}
	}

	isSucceed = true;
Exit:
	return isSucceed;
}
