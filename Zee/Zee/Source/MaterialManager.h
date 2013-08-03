#ifndef MATERIALMANAGER_H
#define MATERIALMANAGER_H

#include "Material.h"

class MaterialManager
{
public:
	static void Init();

	static void AddMaterial(Material* material);
	static void DeleteAll();
	static void GetMaterial(const wchar_t* name, Material** material);

private:
	static std::list<Material*> resourceList;
	static DWORD curID;
};


#endif