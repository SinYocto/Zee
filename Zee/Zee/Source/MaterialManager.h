#ifndef MATERIALMANAGER_H
#define MATERIALMANAGER_H

#include "Material.h"
#include <list>

class MaterialManager
{
public:
	static void AddMaterial(Material* material);
	static void DeleteAll();
	static void GetMaterial(const wchar_t* name, Material** material);

private:
	static std::list<Material*> resourceList;
	static DWORD curIDIndex; 
};


#endif