#ifndef UTILITY_H
#define UTILITY_H

#include <stdio.h>
#include <io.h>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <string.h>
#include <string>
#include <malloc.h>
#include <assert.h>
#include <Windows.h>

#define Assert(condition) \
	if(!(condition)) \
	{ \
		{ \
		wprintf(L"File: %S(%d) has triggered a break point.\nAssert(%S) failed!\n", \
		__FILE__, __LINE__, #condition); \
		__asm int 3 \
		} \
		goto Exit; \
	}

#define _Assert(condition) \
	if(!(condition)) \
	{ \
		{ \
		wprintf(L"File: %S(%d) has triggered a break point.\nAssert(%S) failed!\n", \
		__FILE__, __LINE__, #condition); \
		__asm int 3 \
		} \
	}

#define Log wprintf

#define New ::new(_NORMAL_BLOCK, __FILE__, __LINE__)

#define SAFE_RELEASE(p)	{ if(p) { (p)->Release(); (p)=NULL; } }
#define SAFE_DELETE(p) { if(p) { delete (p); (p) = NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p); (p) = NULL; } }
#define SAFE_DROP(p)	{ if(p) { (p)->Drop(); (p)=NULL; } }

#define MAX_STR_LEN 1024
#define MAX_PATH_LEN 1024

template<class _Type>
void ZeroStruct(_Type& refObj)
{
	::memset((void*)&refObj, 0, sizeof(_Type));
}

#endif