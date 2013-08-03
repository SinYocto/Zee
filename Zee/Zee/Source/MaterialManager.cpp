#include "MaterialManager.h"

std::list<Material*> MaterialManager::resourceList;
DWORD MaterialManager::curID = 0;

void MaterialManager::AddMaterial(Material* material)
{
	_Assert(NULL != material);

	material->SetID(curID++);			// QUESTION:ID一直加不会溢出吧
	resourceList.push_back(material);
}

void MaterialManager::DeleteAll()
{
	for(std::list<Material*>::iterator iter = resourceList.begin(); iter != resourceList.end(); ++iter)
	{
		SAFE_DROP(*iter);		// 这里使用了drop方法而不是delete, 当有Model持有material对象时, 虽然material对象不在GeometryManager中管理了
								// 但Model中仍能正常持有 
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

void MaterialManager::Init()
{
	// QUESTION:effect要不要release?
	DiffuseShader::CreateEffectFromFile(TEXT("./Source/Shaders/Diffuse.fx"));	// time used: 124ms
	_Assert(NULL != DiffuseShader::mEffect);

	SpecularShader::CreateEffectFromFile(TEXT("./Source/Shaders/Specular.fx"));		// time used: 230ms
	_Assert(NULL != SpecularShader::mEffect);

	BumpSpecularShader::CreateEffectFromFile(TEXT("./Source/Shaders/BumpSpecular.fx"));		// time used: 248ms
	_Assert(NULL != BumpSpecularShader::mEffect);
}
