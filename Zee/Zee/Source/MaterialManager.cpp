#include "MaterialManager.h"

std::list<Material*> MaterialManager::mResourceList;
DWORD MaterialManager::mCurID = 0;

void MaterialManager::AddMaterial(Material* material)
{
	_Assert(NULL != material);

	material->SetID(mCurID++);			// QUESTION:ID一直加不会溢出吧
	mResourceList.push_back(material);
}

void MaterialManager::DeleteAll()
{
	for(std::list<Material*>::iterator iter = mResourceList.begin(); iter != mResourceList.end(); ++iter)
	{
		SAFE_DROP(*iter);		// 这里使用了drop方法而不是delete, 当有Model持有material对象时, 虽然material对象不在GeometryManager中管理了
								// 但Model中仍能正常持有 
	}

	mResourceList.clear();
}

void MaterialManager::GetMaterial(const wchar_t* name, Material** material)
{
	_Assert(NULL != material);

	*material = NULL;
	for(std::list<Material*>::iterator iter = mResourceList.begin(); iter != mResourceList.end(); ++iter)
	{
		Material* curMaterial = *iter;
		if(YString::Compare(curMaterial->GetName(), name) == 0)
		{
			*material = curMaterial;
			break;
		}
	}
}