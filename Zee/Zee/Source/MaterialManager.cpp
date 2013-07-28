#include "MaterialManager.h"

std::list<Material*> MaterialManager::mResourceList;
DWORD MaterialManager::mCurID = 0;

void MaterialManager::AddMaterial(Material* material)
{
	_Assert(NULL != material);

	material->SetID(mCurID++);			// QUESTION:IDһֱ�Ӳ��������
	mResourceList.push_back(material);
}

void MaterialManager::DeleteAll()
{
	for(std::list<Material*>::iterator iter = mResourceList.begin(); iter != mResourceList.end(); ++iter)
	{
		SAFE_DROP(*iter);		// ����ʹ����drop����������delete, ����Model����material����ʱ, ��Ȼmaterial������GeometryManager�й�����
								// ��Model�������������� 
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