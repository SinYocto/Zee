#include "GeometryManager.h"

std::list<Geometry*> GeometryManager::mResourceList;
DWORD GeometryManager::mCurID = 0;

void GeometryManager::AddGeometry(Geometry* geo)
{
	_Assert(NULL != geo);

	geo->SetID(mCurID++);			// QUESTION:IDһֱ�Ӳ��������
	mResourceList.push_back(geo);
}

void GeometryManager::DeleteAll()
{
	for(std::list<Geometry*>::iterator iter = mResourceList.begin(); iter != mResourceList.end(); ++iter)
	{
		SAFE_DROP(*iter);		// ����ʹ����drop����������delete, ����Model����geo����ʱ, ��Ȼgeo������GeometryManager�й�����
								// ��Model��������������
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
