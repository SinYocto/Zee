#include "GeometryManager.h"
#include "Engine.h"

GeometryManager::GeometryManager()
{

}

void GeometryManager::AddGeometry(Geometry* geo)
{
	_Assert(NULL != geo);

	_Assert(!YString::isEmpty(geo->GetFilePath()));
	GeoHashMap::iterator iter = mGeometries.find(geo->GetFilePath());
	if(iter != mGeometries.end())
	{
		Log(L"warning: GeometryManage::AddGeometry() geo already exists!\n");
		return;
	}
	else
	{
		mGeometries[geo->GetFilePath()] = geo;
	}
}

void GeometryManager::Destroy()
{
	for(GeoHashMap::iterator iter = mGeometries.begin(); iter != mGeometries.end(); ++iter)
	{
		SAFE_DROP(iter->second);
	}

	mGeometries.clear();
}

void GeometryManager::OnLostDevice()
{
	for(GeoHashMap::iterator iter = mGeometries.begin(); iter != mGeometries.end(); ++iter)
	{
		Geometry* geo = iter->second;
		geo->OnLostDevice();
	}
}

void GeometryManager::OnResetDevice()
{
	for(GeoHashMap::iterator iter = mGeometries.begin(); iter != mGeometries.end(); ++iter)
	{
		Geometry* geo = iter->second;
		geo->OnResetDevice();
	}
}

Geometry* GeometryManager::GetOrCreateGeometry(const wchar_t* filePath)
{
	Geometry* resultGeo = NULL;

	GeoHashMap::iterator iter = mGeometries.find(filePath);
	if(iter != mGeometries.end())
	{
		resultGeo = iter->second;
		_Assert(NULL != resultGeo);
	}
	else
	{
		_Assert(YFile::Exist(filePath));

		wchar_t fileName[MAX_STR_LEN];
		YString::GetFileName(fileName, _countof(fileName), filePath, false);

		resultGeo = New Geometry(fileName, filePath);

		mGeometries[filePath] = resultGeo;
	}

	return resultGeo;
}

GeoHashMap GeometryManager::GetGeoHashMap()
{
	return mGeometries;
}
