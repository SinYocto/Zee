#ifndef RESOURCE_MGR_H
#define RESOURCE_MGR_H

#include "Common.h"

class Billboard;

class ResourceMgr
{
public:
	void Destroy();

	void OnLostDevice();
	void OnResetDevice();

	void AddBillboard(Billboard* billboard);

private:
	std::list<Billboard*> billboardList;
};

#endif