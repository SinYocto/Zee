#ifndef UTILITY_H
#define UTILITY_H

#include <stdio.h>

#define Assert(condition) \
	if(!(condition)) \
	{ \
		{ \
		printf("File: %s(%d) has triggered a break point.\nAssert(%s) failed!\n", \
		__FILE__, __LINE__, #condition); \
		__asm int 3 \
		} \
		goto Exit; \
	}

#define _Assert(condition) \
	if(!(condition)) \
	{ \
		{ \
		printf("File: %s(%d) has triggered a break point.\nAssert(%s) failed!\n", \
		__FILE__, __LINE__, #condition); \
		__asm int 3 \
		} \
	}

#define Log printf

#define SAFE_RELEASE(p)	{ if(p) { (p)->Release(); (p)=NULL; } }
#define SAFE_DELETE(p) { if(p) { delete (p); (p) = NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p); (p) = NULL; } }
#define SAFE_DROP(p)	{ if(p) { (p)->Drop(); (p)=NULL; } }

#define MAX_STR_LEN 1024
#define MAX_PATH_LEN 1024

#endif