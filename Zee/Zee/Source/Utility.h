#ifndef UTILITY_H
#define UTILITY_H

#include <stdio.h>
#include <vector>
#include <list>

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

#define SAFE_RELEASE(p)	{ if(p) { (p)->Release(); (p)=NULL; } }
#define SAFE_DELETE(p) { if(p) { delete (p); (p) = NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p); (p) = NULL; } }
#define SAFE_DROP(p)	{ if(p) { (p)->Drop(); (p)=NULL; } }

#define MAX_STR_LEN 1024
#define MAX_PATH_LEN 1024

#endif