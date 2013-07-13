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

	static YFile* Open(const char* filePath, OPEN_MODE mode);

	char* ReadLine(char* lineContent, int maxLineSize);
	bool ReadBlock(std::vector<std::string>* blockContent, const char* beginSpecifier, const char* endSpecifier = NULL);

	bool Close();

private:
	bool reachEnd();

private:
	FILE* pFile;
};

#endif