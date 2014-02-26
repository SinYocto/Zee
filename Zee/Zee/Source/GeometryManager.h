#ifndef GEOMETRYMANAGER_H
#define GEOMETRYMANAGER_H

#include "Geometry.h"
#include "HashMap.h"

typedef HashMap<Geometry> GeoHashMap;

class GeometryManager
{
public:
	GeometryManager();

	void OnLostDevice();
	void OnResetDevice();

	void Destroy();

	Geometry* GetOrCreateGeometry(const wchar_t* filePath);
	void AddGeometry(Geometry* geo);

	std::vector<Geometry*> GetAllGeometries();

private:
	std::list<Geometry*> geoList;
	GeoHashMap mGeometries;
};


#endif