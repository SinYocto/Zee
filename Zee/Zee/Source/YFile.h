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
		READ_BINARY,
		WRITE,
		WRITE_BINARY,
		APPEND,
	};

public:
	YFile()
		:pFile(NULL)
	{

	}

	static YFile* Open(const wchar_t* filePath, OPEN_MODE mode);

	int Read(void* destBuffer, int destSize, int size, int count);
	wchar_t* ReadLine(wchar_t* lineContent, int maxLineSize);
	int ReadBlock(std::vector<std::wstring>* blockContent, const wchar_t* beginSpecifier, 
		const wchar_t* endSpecifier = NULL, const wchar_t* firstLineContent = NULL);

	int WriteLine(const wchar_t* format, ...);
	int Write(const void* ptr, int size, int count);

	static bool Exist(const wchar_t* filePath);

	bool Close();
	bool ReachEnd();

public:
	FILE* pFile;
};

#endif