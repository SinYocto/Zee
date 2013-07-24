#ifndef IREFERENCECOUNTED_H
#define IREFERENCECOUNTED_H

#include "Utility.h"

/* ���ü�������, ��Ҫʹ�����ü�����������Դ�ͷŵ�����Լ̳д���
 * Ŀǰʹ�õ���Geometry���Material��
 * ÿ������һ�ε���Grab����, �ͷ����õ���Drop����
 */


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