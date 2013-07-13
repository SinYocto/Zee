#ifndef YSTRING_H
#define YSTRING_H

#include <stdarg.h>

class YString
{
public:
	static bool Empty(char* str);

	static bool Copy(char* destStr, int destStrSize, const char* srcStr);
	static bool Concat(char* destStr, int destStrSize, const char* srcStr);

	static bool GetSpecifier(char* specifier, int specifierSize, const char* str);

	static int Compare(const char* str1, const char* str2, bool ignoreCase = false);
	static int Scan(const char* str, const char* format, ...);

private:
	static int vsscanf(const char* str, const char* format, va_list argPtr);
};

#endif