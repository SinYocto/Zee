#ifndef IREFERENCECOUNTED_H
#define IREFERENCECOUNTED_H

#include "D3DUtility.h"

class IReferenceCounted
{
public:
	IReferenceCounted()
		:referenceCounter(1)
	{

	}

	virtual ~IReferenceCounted()	// 因为Drop方法中有delete this操作, 必须要定义虚析构函数
	{

	}


	void Grab()
	{
		++referenceCounter;
	}

	void Drop()
	{
		_Assert(referenceCounter > 0);

		if(--referenceCounter == 0)
		{
			delete this;		// QUESTION:如果对象不是new出来的呢?
		}
	}

public:
	int referenceCounter;
};

#endif