#ifndef MATERIALMANAGER_H
#define MATERIALMANAGER_H

#include "Material.h"
#include "ResourceHashMap.h"

typedef stdext::hash_map<const wchar_t*, Material*, stdext::hash_compare<const wchar_t*, WCharLess>> MtlHashMap;

class MaterialManager
{
public:
	MaterialManager();

	void Init();
	void Destroy();

	void OnLostDevice();
	void OnResetDevice();

	void FrameUpdate();

	Material* GetOrCreateMaterial(const wchar_t* filePath);
	void AddMaterial(Material* material);

	Material* GetDefaultFlatMtl();
	Material* GetDefaultViewMtl();
	Material* GetDefaultDiffMtl();
	Material* GetDefaultSpecMtl();

	MtlHashMap GetMtlHashMap();

private:
	MtlHashMap mMaterials;
};


#endif