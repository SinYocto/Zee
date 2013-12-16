#ifndef GEOMETRYMANAGER_H
#define GEOMETRYMANAGER_H

#include "Geometry.h"
#include "ResourceHashMap.h"

typedef stdext::hash_map<const wchar_t*, Geometry*, stdext::hash_compare<const wchar_t*, WCharLess>> GeoHashMap;

class GeometryManager
{
public:
	GeometryManager();

	void OnLostDevice();
	void OnResetDevice();

	void Destroy();

	Geometry* GetOrCreateGeometry(const wchar_t* filePath);
	void AddGeometry(Geometry* geo);

	GeoHashMap GetGeoHashMap();

private:
	std::list<Geometry*> geoList;
	GeoHashMap mGeometries;
};


#endif