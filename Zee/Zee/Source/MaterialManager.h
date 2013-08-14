#ifndef MATERIALMANAGER_H
#define MATERIALMANAGER_H

#include "Material.h"

class MaterialManager
{
public:
	static void Init();

	static void OnLostDevice();
	static void OnResetDevice();

	static void FrameUpdate();

	static void AddMaterial(Material* material);
	static void DeleteAll();
	static void GetMaterial(const wchar_t* name, Material** material);

private:
	static void createDefaultMtls();
	static void deleteDefaultMtls();

private:
	static std::list<Material*> resourceList;
	static DWORD curID;

public:
	static Material* flatMtl;
	static Material* viewMtl;
	static Material* diffMtl;
	static Material* specMtl;
};


#endif