#ifndef YFILE_H
#define YFILE_H

#include <stdio.h>
#include <vector>

class YFile
{
public:
	enum OPEN_MODE
	{
		READ,
		WRITE
	};

public:
	YFile()
		:pFile(NULL)
	{

	}

	static YFile* Open(const wchar_t* filePath, OPEN_MODE mode);

	wchar_t* ReadLine(wchar_t* lineContent, int maxLineSize);
	int ReadBlock(std::vector<std::wstring>* blockContent, const wchar_t* beginSpecifier, const wchar_t* endSpecifier = NULL);

	bool Close();

private:
	bool reachEnd();

public:
	FILE* pFile;
};

#endif