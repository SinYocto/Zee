#include "GeometryManager.h"

std::list<Geometry*> GeometryManager::resourceList;
DWORD GeometryManager::curIDIndex = 0;

void GeometryManager::AddGeometry(Geometry* geo)
{
	_Assert(NULL != geo);

	geo->SetID(curIDIndex++);			// QUESTION:IDһֱ�Ӳ��������
	resourceList.push_back(geo);
}

void GeometryManager::DeleteAll()
{
	for(std::list<Geometry*>::iterator iter = resourceList.begin(); iter != resourceList.end(); ++iter)
	{
		SAFE_DROP(*iter);		// ����ʹ����drop����������delete, ����Model����geo����ʱ, ��Ȼgeo������GeometryManager�й�����
								// ��Model��������������
	}

	resourceList.clear();
}

void GeometryManager::GetGeometry(const wchar_t* name, Geometry** geo)
{
	_Assert(NULL != geo);

	*geo = NULL;
	for(std::list<Geometry*>::iterator iter = resourceList.begin(); iter != resourceList.end(); ++iter)
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
	for(std::list<Geometry*>::iterator iter = resourceList.begin(); iter != resourceList.end(); ++iter)
	{
		(*iter)->OnLostDevice();
	}
}

void GeometryManager::OnResetDevice()
{
	for(std::list<Geometry*>::iterator iter = resourceList.begin(); iter != resourceList.end(); ++iter)
	{
		(*iter)->OnResetDevice();
	}
}
