#ifndef RESOURCE_MGR_H
#define RESOURCE_MGR_H

#include "Billboard.h"

class ResourceMgr
{
public:
	static void OnLostDevice();
	static void OnResetDevice();

	static void AddBillboard(Billboard* billboard);

private:
	static std::list<Billboard*> billboardList;
};

#endif