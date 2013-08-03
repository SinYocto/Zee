#ifndef IREFERENCECOUNTED_H
#define IREFERENCECOUNTED_H

#include "Common.h"

/* 引用计数基类, 需要使用引用计数来管理资源释放的类可以继承此类
 * 目前使用的有Geometry类和Material类
 * 每被引用一次调用Grab方法, 释放引用调用Drop方法
 */


class IReferenceCounted
{
public:
	IReferenceCounted()
		:mRefCounts(1)
	{

	}

	virtual ~IReferenceCounted()	// 因为Drop方法中有delete this操作, 必须要定义虚析构函数
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
			delete this;		// QUESTION:如果对象不是new出来的呢?
		}
	}

public:
	int mRefCounts;
};

#endif