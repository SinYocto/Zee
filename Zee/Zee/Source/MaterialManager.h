#ifndef MATERIALMANAGER_H
#define MATERIALMANAGER_H

#include "Material.h"

class MaterialManager
{
public:
	MaterialManager();

	void Init();
	void Destroy();

	void OnLostDevice();
	void OnResetDevice();

	void FrameUpdate();

	void AddMaterial(Material* material);
	void GetMaterial(const wchar_t* name, Material** material);

	std::list<Material*> GetMaterialList();

	Material* GetDefaultFlatMtl();
	Material* GetDefaultViewMtl();
	Material* GetDefaultDiffMtl();
	Material* GetDefaultSpecMtl();

private:
	void createDefaultMtls();
	void deleteDefaultMtls();

private:
	std::list<Material*> materialList;
	
	Material* flatMtl;
	Material* viewMtl;
	Material* diffMtl;
	Material* specMtl;
};


#endif