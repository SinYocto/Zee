#include "MaterialManager.h"

std::list<Material*> MaterialManager::resourceList;
DWORD MaterialManager::curIDIndex = 0;

void MaterialManager::AddMaterial(Material* material)
{
	_Assert(NULL != material);

	material->SetID(curIDIndex++);			// QUESTION:IDһֱ�Ӳ��������
	resourceList.push_back(material);
}

void MaterialManager::DeleteAll()
{
	for(std::list<Material*>::iterator iter = resourceList.begin(); iter != resourceList.end(); ++iter)
	{
		SAFE_DROP(*iter);		// ����ʹ����drop����������delete, ����Model����material����ʱ, ��Ȼmaterial������MeshManager�й�����
								// ��Model�������������� 
	}

	resourceList.clear();
}

void MaterialManager::GetMaterial(const wchar_t* name, Material** material)
{
	_Assert(NULL != material);

	*material = NULL;
	for(std::list<Material*>::iterator iter = resourceList.begin(); iter != resourceList.end(); ++iter)
	{
		Material* curMaterial = *iter;
		if(YString::Compare(curMaterial->GetName(), name) == 0)
		{
			*material = curMaterial;
			break;
		}
	}
}