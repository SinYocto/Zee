#include "HashMap.h"

unsigned int DJBHash(wchar_t* key)
{
	unsigned int hashValue = 0;
	while(*key)
		hashValue = (hashValue << 5) + hashValue + *key++;

	return hashValue;
}
