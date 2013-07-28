#include "GeometryManager.h"

std::list<Geometry*> GeometryManager::mResourceList;
DWORD GeometryManager::mCurID = 0;

void GeometryManager::AddGeometry(Geometry* geo)
{
	_Assert(NULL != geo);

	geo->SetID(mCurID++);			// QUESTION:ID一直加不会溢出吧
	mResourceList.push_back(geo);
}

void GeometryManager::DeleteAll()
{
	for(std::list<Geometry*>::iterator iter = mResourceList.begin(); iter != mResourceList.end(); ++iter)
	{
		SAFE_DROP(*iter);		// 这里使用了drop方法而不是delete, 当有Model持有geo对象时, 虽然geo对象不在GeometryManager中管理了
								// 但Model中仍能正常持有
	}

	mResourceList.clear();
}

void GeometryManager::GetGeometry(const wchar_t* name, Geometry** geo)
{
	_Assert(NULL != geo);

	*geo = NULL;
	for(std::list<Geometry*>::iterator iter = mResourceList.begin(); iter != mResourceList.end(); ++iter)
	{
		Geometry* curGeo = *iter;
		if(YString::Compare(curGeo->GetName(), name) == 0)
		{
			*geo = curGeo;
			break;
		}
	}
}

void GeometryManager::OnLostDevice()
{
	for(std::list<Geometry*>::iterator iter = mResourceList.begin(); iter != mResourceList.end(); ++iter)
	{
		(*iter)->OnLostDevice();
	}
}

void GeometryManager::OnResetDevice()
{
	for(std::list<Geometry*>::iterator iter = mResourceList.begin(); iter != mResourceList.end(); ++iter)
	{
		(*iter)->OnResetDevice();
	}
}
