#ifndef HASH_MAP_H
#define HASH_MAP_H

#include "Utility.h"
#include "YString.h"

unsigned int DJBHash(wchar_t* key);

template <class DATA> class HashMap
{
	struct HashNode
	{
		HashNode(const wchar_t* str)
			:data(NULL)
			,next(NULL)
		{
			YString::Copy(key, _countof(key), str);
		}

		wchar_t key[MAX_STR_LEN];
		DATA* data;
		HashNode* next;
	};

public:
	HashMap(int size = 1024)
		:mSize(size)
	{
		for(int i = 0; i < size; ++i)
		{
			HashNode* header = NULL;
			mHashTable.push_back(header);
		}
	}

	void Insert(const wchar_t* key, DATA* data)
	{
		wchar_t key_lower[MAX_STR_LEN];
		YString::Copy(key_lower, _countof(key_lower), key);
		_wcslwr_s(key_lower, _countof(key_lower));

		int hashVal = DJBHash(key_lower) % mSize;

		HashNode*& node = mHashTable[hashVal];
		HashNode* prevNode = node;

		while(node != NULL)
		{
			node = node->next;
			prevNode = node;
		}

		node = new HashNode(key_lower);		// TODO: delete
		node->data = data;

		if(prevNode != NULL)
			prevNode->next = node;
	}

	DATA* Find(const wchar_t* key)
	{
		wchar_t key_lower[MAX_STR_LEN];
		YString::Copy(key_lower, _countof(key_lower), key);
		_wcslwr_s(key_lower, _countof(key_lower));

		DATA* resultData = NULL;
		int hashVal = DJBHash(key_lower) % mSize;

		HashNode* node = mHashTable[hashVal];

		while(node != NULL)
		{
			if(YString::Compare(key_lower, node->key) == 0)
			{
				resultData = node->data;
				break;
			}

			node = node->next;
		}

		return resultData;
	}

	std::vector<DATA*> GetDataList(int index)	// 使外部访问到所有data的方法, 目前用于需要delete所有data的情况
	{
		_Assert(mSize == (int)mHashTable.size());
		_Assert(index < mSize);

		std::vector<DATA*> dataList;

		HashNode* header = mHashTable[index];

		while(header != NULL)
		{
			dataList.push_back(header->data);
			header = header->next;
		}

		return dataList;
	}

	void Destory()
	{
		int size = (int)mHashTable.size();

		for(int i = 0; i < size; ++i)
		{
			HashNode* header = mHashTable[i];

			while(header != NULL)
			{
				HashNode* next = header->next;
				delete header;

				header = next;
			}
		}
	}

	int GetSize()
	{
		return mSize;
	}

private:
	int mSize;
	std::vector<HashNode*> mHashTable;
};

#endif