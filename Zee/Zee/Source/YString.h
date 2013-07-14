#ifndef YSTRING_H
#define YSTRING_H

#include "Utility.h"
#include <stdarg.h>

class YString
{
public:
	static bool isEmpty(wchar_t* str);

	static bool Empty(char* str);
	static bool Empty(wchar_t* str);

	static bool Copy(char* destStr, int destStrSize, const char* srcStr);
	static bool Copy(wchar_t* destStr, int destStrSize, const wchar_t* srcStr);

	static bool Concat(char* destStr, int destStrSize, const char* srcStr);
	static bool Concat(wchar_t* destStr, int destStrSize, const wchar_t* srcStr);

	static bool GetSpecifier(char* specifier, int specifierSize, const char* str);
	static bool GetSpecifier(wchar_t* specifier, int specifierSize, const wchar_t* str);

	static int Compare(const char* str1, const char* str2, bool ignoreCase = false);
	static int Compare(const wchar_t* str1, const wchar_t* str2, bool ignoreCase = false);

	static int Scan(const char* str, const char* format, ...);
	static int Scan(const wchar_t* str, const wchar_t* format, ...);

	// path
	static bool GetCurrentDirPath(wchar_t* curDirPath, int strSize);
	static bool GetFullPath(wchar_t* fullPath, int fullPathSize, const wchar_t* relativePath = NULL);
	static bool NormalizePath(wchar_t* path);

private:
	static int vsscanf(const char* str, const char* format, va_list argPtr);
	static int vsscanf(const wchar_t* str, const wchar_t* format, va_list argPtr);
};

#endif