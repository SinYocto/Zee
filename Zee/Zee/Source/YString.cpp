#include "YString.h"
#include "Utility.h"

#include <stdio.h>
#include <malloc.h>
#include <string.h>

bool YString::Empty(char* str)
{
	Assert(NULL != str);
	str[0] = (char)'\0';

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

bool YString::Concat(char* destStr, int destStrSize, const char* srcStr)
{
	Assert(NULL != destStr);
	Assert(NULL != srcStr);

	strcat_s(destStr, destStrSize, srcStr);

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

// HACK:http://stackoverflow.com/questions/2457331/replacement-for-vsscanf-on-msvc
int YString::vsscanf(const char* str, const char* format, va_list argPtr)
{
	void *args[20];

	for (int i = 0; i < sizeof(args) / sizeof(args[0]); ++i) 
		args[i] = va_arg(argPtr, void*);

	return sscanf(str, format, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9], args[10], 
		args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19], args[20]);
}
