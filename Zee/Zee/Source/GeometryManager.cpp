#include "GeometryManager.h"
#include "Engine.h"

GeometryManager::GeometryManager()
{

}

void GeometryManager::AddGeometry(Geometry* geo)
{
	_Assert(NULL != geo);

	gEngine->GetIDAllocator()->AllocateGeometryID(geo);
	geoList.push_back(geo);
}

void GeometryManager::Destroy()
{
	for(std::list<Geometry*>::iterator iter = geoList.begin(); iter != geoList.end(); ++iter)
	{
		SAFE_DROP(*iter);		// 这里使用了drop方法而不是delete, 当有Model持有geo对象时, 虽然geo对象不在GeometryManager中管理了
								// 但Model中仍能正常持有
	}

	geoList.clear();
}

void GeometryManager::GetGeometry(const wchar_t* name, Geometry** geo)
{
	_Assert(NULL != geo);

	*geo = NULL;
	for(std::list<Geometry*>::iterator iter = geoList.begin(); iter != geoList.end(); ++iter)
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
	for(std::list<Geometry*>::iterator iter = geoList.begin(); iter != geoList.end(); ++iter)
	{
		(*iter)->OnLostDevice();
	}
}

void GeometryManager::OnResetDevice()
{
	for(std::list<Geometry*>::iterator iter = geoList.begin(); iter != geoList.end(); ++iter)
	{
		(*iter)->OnResetDevice();
	}
}