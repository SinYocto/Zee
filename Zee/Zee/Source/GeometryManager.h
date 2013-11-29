#ifndef GEOMETRYMANAGER_H
#define GEOMETRYMANAGER_H

#include "Geometry.h"

class GeometryManager
{
public:
	GeometryManager();

	void Destroy();

	void AddGeometry(Geometry* geo);
	void GetGeometry(const wchar_t* name, Geometry** geo);

	void OnLostDevice();
	void OnResetDevice();

private:
	std::list<Geometry*> geoList;
};


#endif