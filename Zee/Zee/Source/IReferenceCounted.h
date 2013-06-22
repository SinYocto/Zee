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

	virtual ~IReferenceCounted()	// ��ΪDrop��������delete this����, ����Ҫ��������������
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
			delete this;		// QUESTION:���������new��������?
		}
	}

public:
	int referenceCounter;
};

#endif