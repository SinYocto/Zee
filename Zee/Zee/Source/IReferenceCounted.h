#ifndef IREFERENCECOUNTED_H
#define IREFERENCECOUNTED_H

#include "Common.h"

/* ���ü�������, ��Ҫʹ�����ü�����������Դ�ͷŵ�����Լ̳д���
 * Ŀǰʹ�õ���Geometry���Material��
 * ÿ������һ�ε���Grab����, �ͷ����õ���Drop����
 */


class IReferenceCounted
{
public:
	IReferenceCounted()
		:mRefCounts(1)
	{

	}

	virtual ~IReferenceCounted()	// ��ΪDrop��������delete this����, ����Ҫ��������������
	{

	}


	void Grab()
	{
		++mRefCounts;
	}

	void Drop()
	{
		_Assert(mRefCounts > 0);

		if(--mRefCounts == 0)
		{
			delete this;		// QUESTION:���������new��������?
		}
	}

public:
	int mRefCounts;
};

#endif