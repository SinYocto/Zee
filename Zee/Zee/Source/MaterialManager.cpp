#include "MaterialManager.h"

std::list<Material*> MaterialManager::resourceList;
DWORD MaterialManager::curIDIndex = 0;

void MaterialManager::AddMaterial(Material* material)
{
	_Assert(NULL != material);

	material->SetID(curIDIndex++);			// QUESTION:ID一直加不会溢出吧
	resourceList.push_back(material);
}

void MaterialManager::DeleteAll()
{
	for(std::list<Material*>::iterator iter = resourceList.begin(); iter != resourceList.end(); ++iter)
	{
		SAFE_DROP(*iter);
	}

	resourceList.clear();
}

void MaterialManager::GetMaterial(const char* name, Material** material)
{
	_Assert(NULL != material);

	*material = NULL;
	for(std::list<Material*>::iterator iter = resourceList.begin(); iter != resourceList.end(); ++iter)
	{
		Material* curMaterial = *iter;
		if(strcmp(curMaterial->GetName(), name) == 0)
		{
			*material = curMaterial;
			break;
		}
	}
}