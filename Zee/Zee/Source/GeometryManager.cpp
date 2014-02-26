#include "GeometryManager.h"
#include "Engine.h"

GeometryManager::GeometryManager()
{

}

void GeometryManager::AddGeometry(Geometry* geo)
{
	_Assert(NULL != geo);

	//_Assert(!YString::isEmpty(geo->GetFilePath()));
	if(mGeometries.Find(geo->GetFilePath()) != NULL)
	{
		Log(L"warning: GeometryManage::AddGeometry() geo already exists!\n");
		return;
	}
	else
	{
		mGeometries.Insert(geo->GetFilePath(), geo);
	}
}

void GeometryManager::Destroy()
{
	int size = mGeometries.GetSize();
	for(int i = 0; i < size; ++i)
	{
		std::vector<Geometry*> geoList = mGeometries.GetDataList(i);

		for(std::vector<Geometry*>::iterator iter = geoList.begin(); iter != geoList.end(); ++iter)
		{
			SAFE_DROP(*iter);
		}
	}

	mGeometries.Destory();
}

void GeometryManager::OnLostDevice()
{
	int size = mGeometries.GetSize();
	for(int i = 0; i < size; ++i)
	{
		std::vector<Geometry*> geoList = mGeometries.GetDataList(i);

		for(std::vector<Geometry*>::iterator iter = geoList.begin(); iter != geoList.end(); ++iter)
		{
			if(*iter)
				(*iter)->OnLostDevice();
		}
	}
}

void GeometryManager::OnResetDevice()
{
	int size = mGeometries.GetSize();
	for(int i = 0; i < size; ++i)
	{
		std::vector<Geometry*> geoList = mGeometries.GetDataList(i);

		for(std::vector<Geometry*>::iterator iter = geoList.begin(); iter != geoList.end(); ++iter)
		{
			if(*iter)
				(*iter)->OnResetDevice();
		}
	}
}

Geometry* GeometryManager::GetOrCreateGeometry(const wchar_t* filePath)
{
	Geometry* resultGeo = NULL;

	resultGeo = mGeometries.Find(filePath);
	if(resultGeo == NULL)
	{
		_Assert(YFile::Exist(filePath));

		wchar_t fileName[MAX_STR_LEN];
		YString::GetFileName(fileName, _countof(fileName), filePath, false);

		resultGeo = New Geometry(fileName, filePath);

		mGeometries.Insert(filePath, resultGeo);
	}

	return resultGeo;
}

std::vector<Geometry*> GeometryManager::GetAllGeometries()
{
	std::vector<Geometry*> totalGeoList;

	int size = mGeometries.GetSize();
	for(int i = 0; i < size; ++ i)
	{
		std::vector<Geometry*> geoList = mGeometries.GetDataList(i);
		for(std::vector<Geometry*>::iterator iter = geoList.begin(); iter != geoList.end(); ++iter)
		{
			if(*iter != NULL)
				totalGeoList.push_back(*iter);
		}
	}

	return totalGeoList;
}
