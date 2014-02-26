#ifndef MATERIALMANAGER_H
#define MATERIALMANAGER_H

#include "Material.h"
#include "HashMap.h"

typedef HashMap<Material> MtlHashMap;

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

	std::vector<Material*> GetAllMaterials();

private:
	MtlHashMap mMaterials;
};


#endif