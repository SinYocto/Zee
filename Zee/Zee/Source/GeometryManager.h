#ifndef GEOMETRYMANAGER_H
#define GEOMETRYMANAGER_H

#include "Geometry.h"

class GeometryManager
{
public:
	static void AddGeometry(Geometry* geo);
	static void Destroy();
	static void GetGeometry(const wchar_t* name, Geometry** geo);

	static void OnLostDevice();
	static void OnResetDevice();

private:
	static std::list<Geometry*> geoList;
	static DWORD curID;
};


#endif