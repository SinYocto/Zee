#include "YString.h"
#include "Utility.h"

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <Windows.h>

bool YString::isEmpty(wchar_t* str)
{
	Assert(NULL != str);

	return str[0] == 0;
Exit:
	return true;
}

bool YString::Empty(char* str)
{
	Assert(NULL != str);
	str[0] = (char)'\0';

	return true;
Exit:
	return false;
}

bool YString::Empty(wchar_t* str)
{
	Assert(NULL != str);
	str[0] = 0;

	return true;
Exit:
	return false;
}

bool YString::Copy(char* destStr, int destStrSize, const char* srcStr)
{
	Assert(NULL != srcStr);
	Assert(NULL != destStr);

	strcpy_s(destStr, destStrSize, srcStr);

	return true;
Exit:
	return false;
}

bool YString::Copy(wchar_t* destStr, int destStrSize, const wchar_t* srcStr)
{
	Assert(NULL != srcStr);
	Assert(NULL != destStr);

	wcscpy_s(destStr, destStrSize, srcStr);

	return true;
Exit:
	return false;
}

int YString::Compare(const char* str1, const char* str2, bool ignoreCase /*= false*/)
{
	Assert(NULL != str1);
	Assert(NULL != str2);

	if (ignoreCase)
	{
		return _stricmp(str1, str2);
	}
	else
	{
		return strcmp(str1, str2);
	}

Exit:
	return -1;
}

int YString::Compare(const wchar_t* str1, const wchar_t* str2, bool ignoreCase /*= false*/)
{
	Assert(NULL != str1);
	Assert(NULL != str2);

	if (ignoreCase)
	{
		return _wcsicmp(str1, str2);
	}
	else
	{
		return wcscmp(str1, str2);
	}

Exit:
	return -1;
}

bool YString::Concat(char* destStr, int destStrSize, const char* srcStr)
{
	Assert(NULL != destStr);
	Assert(NULL != srcStr);

	strcat_s(destStr, destStrSize, srcStr);

	return true;
Exit:
	return false;
}

bool YString::Concat(wchar_t* destStr, int destStrSize, const wchar_t* srcStr)
{
	Assert(NULL != destStr);
	Assert(NULL != srcStr);

	wcscat_s(destStr, destStrSize, srcStr);

	return true;
Exit:
	return false;
}

bool YString::GetSpecifier(char* specifier, int specifierSize, const char* str)
{
	Assert(NULL != str);
	Assert(NULL != specifier);

	sscanf_s(str, "%s", specifier, specifierSize);

	return true;
Exit:
	return false;
}

bool YString::GetSpecifier(wchar_t* specifier, int specifierSize, const wchar_t* str)
{
	Assert(NULL != str);
	Assert(NULL != specifier);

	swscanf_s(str, L"%s", specifier, specifierSize);

	return true;
Exit:
	return false;
}

int YString::Scan(const char* str, const char* format, ...)
{
	Assert(NULL != str);
	Assert(NULL != format);

	int numSucceed = -1;

	va_list args;
	va_start(args, format);
	numSucceed = vsscanf(str, format, args);
	va_end(args);

Exit:
	return numSucceed;
}

int YString::Scan(const wchar_t* str, const wchar_t* format, ...)
{
	Assert(NULL != str);
	Assert(NULL != format);

	int numSucceed = -1;

	va_list args;
	va_start(args, format);
	numSucceed = vsscanf(str, format, args);
	va_end(args);

Exit:
	return numSucceed;
}

// HACK:http://stackoverflow.com/questions/2457331/replacement-for-vsscanf-on-msvc
int YString::vsscanf(const char* str, const char* format, va_list argPtr)
{
	void *args[20];

	for (int i = 0; i < sizeof(args) / sizeof(args[0]); ++i) 
		args[i] = va_arg(argPtr, void*);

	return sscanf(str, format, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9], args[10], 
		args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19], args[20]);
}

int YString::vsscanf(const wchar_t* str, const wchar_t* format, va_list argPtr)
{
	void *args[20];

	for (int i = 0; i < sizeof(args) / sizeof(args[0]); ++i) 
		args[i] = va_arg(argPtr, void*);

	return swscanf(str, format, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9], args[10], 
		args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19], args[20]);
}

bool YString::GetCurrentDirPath(wchar_t* curDirPath, int strSize)
{
	bool isSucceed = false;
	Assert(GetCurrentDirectory(strSize, curDirPath) != 0);
	Assert(NormalizePath(curDirPath));

	isSucceed = true;
Exit:
	return isSucceed;
}

bool YString::GetFullPath(wchar_t* fullPath, int fullPathSize, const wchar_t* relativePath /*= NULL*/)
{
	bool isSucceed = false;

	wchar_t relativePathUse[MAX_PATH_LEN];
	if(NULL != relativePath)
		YString::Copy(relativePathUse, _countof(relativePathUse), relativePath);
	else
		YString::Copy(relativePathUse, _countof(relativePathUse), fullPath);	// 不传入第3个参数时, 第1个参数传入时就表示相对路径

	Assert(NULL != relativePathUse);
	Assert(GetCurrentDirPath(fullPath, fullPathSize));

	YString::Concat(fullPath, fullPathSize, L"/");				// TODO:兼容不同路径格式规范
	YString::Concat(fullPath, fullPathSize, relativePathUse);

	isSucceed = true;
Exit:
	return isSucceed;
}

bool YString::NormalizePath(wchar_t* path)
{
	bool isSucceed = false;
	Assert(NULL != path);

	wchar_t tempStr[2];
	Copy(tempStr, _countof(tempStr), TEXT("\\"));
	wchar_t backSlashChar = tempStr[0];

	Copy(tempStr, _countof(tempStr), TEXT("/"));
	wchar_t slashChar = tempStr[0];

	wchar_t wc = 0;
	while((wc = path[0]) != 0)
	{
		if(wc == backSlashChar)
			path[0] = slashChar;

		path++;
	}

	isSucceed = true;
Exit:
	return isSucceed;
}
