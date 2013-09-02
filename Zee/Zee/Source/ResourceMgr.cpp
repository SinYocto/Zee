#include "ResourceMgr.h"

std::list<Billboard*> ResourceMgr::billboardList;

void ResourceMgr::OnLostDevice()
{
	for(std::list<Billboard*>::iterator iter = billboardList.begin(); iter != billboardList.end(); ++iter)
	{
		(*iter)->OnLostDevice();
	}
}

void ResourceMgr::OnResetDevice()
{
	for(std::list<Billboard*>::iterator iter = billboardList.begin(); iter != billboardList.end(); ++iter)
	{
		(*iter)->OnResetDevice();
	}
}

void ResourceMgr::AddBillboard(Billboard* billboard)
{
	_Assert(billboard);
	billboardList.push_back(billboard);
}
