#include "MaterialManager.h"
#include "Engine.h"

MaterialManager::MaterialManager()
:flatMtl(NULL)
,viewMtl(NULL)
,diffMtl(NULL)
,specMtl(NULL)
{

}

void MaterialManager::AddMaterial(Material* material)
{
	_Assert(NULL != material);

	gEngine->GetIDAllocator()->AllocateMaterialID(material);
	materialList.push_back(material);
}

void MaterialManager::Destroy()
{
	for(std::list<Material*>::iterator iter = materialList.begin(); iter != materialList.end(); ++iter)
	{
		SAFE_DROP(*iter);		// 这里使用了drop方法而不是delete, 当有Model持有material对象时, 虽然material对象不在GeometryManager中管理了
								// 但Model中仍能正常持有 
	}

	materialList.clear();

	deleteDefaultMtls();
}

void MaterialManager::GetMaterial(const wchar_t* name, Material** material)
{
	_Assert(NULL != material);

	*material = NULL;
	for(std::list<Material*>::iterator iter = materialList.begin(); iter != materialList.end(); ++iter)
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
	UtilityShader::CreateEffectPool();
	UtilityShader::CreateEffectFromFile(TEXT("./Source/Shaders/Utility.fx"));	

	FlatShader::CreateEffectFromFile(TEXT("./Source/Shaders/Flat.fx"));	
	ViewShader::CreateEffectFromFile(TEXT("./Source/Shaders/View.fx"));	
	DiffuseShader::CreateEffectFromFile(TEXT("./Source/Shaders/Diffuse.fx"));	// time used: 124ms
	SpecularShader::CreateEffectFromFile(TEXT("./Source/Shaders/Specular.fx"));		// time used: 230ms
	BumpSpecularShader::CreateEffectFromFile(TEXT("./Source/Shaders/BumpSpecular.fx"));		// time used: 248ms

	createDefaultMtls();
}

void MaterialManager::OnLostDevice()
{
	UtilityShader::OnLostDevice();
	FlatShader::OnLostDevice();
	ViewShader::OnLostDevice();
	DiffuseShader::OnLostDevice();
	SpecularShader::OnLostDevice();
	BumpSpecularShader::OnLostDevice();
}

void MaterialManager::OnResetDevice()
{
	UtilityShader::OnResetDevice();
	FlatShader::OnResetDevice();
	ViewShader::OnResetDevice();
	DiffuseShader::OnResetDevice();
	SpecularShader::OnResetDevice();
	BumpSpecularShader::OnResetDevice();
}

void MaterialManager::FrameUpdate()
{
	UtilityShader::SetupSharedParams();
}

void MaterialManager::createDefaultMtls()
{
	IDAllocator* idAllocator = gEngine->GetIDAllocator();

	flatMtl = New Material(L"defaultFlatMtl");
	flatMtl->SetShader(Flat);
	idAllocator->AllocateMaterialID(flatMtl);

	viewMtl = New Material(L"defaultViewMtl");
	viewMtl->SetShader(View);
	idAllocator->AllocateMaterialID(viewMtl);

	diffMtl = New Material(L"defaultDiffMtl");
	diffMtl->SetShader(Diffuse);
	idAllocator->AllocateMaterialID(diffMtl);

	specMtl = New Material(L"defaultSpecMtl");
	specMtl->SetShader(Specular);
	idAllocator->AllocateMaterialID(specMtl);
}

void MaterialManager::deleteDefaultMtls()
{
	SAFE_DROP(flatMtl);
	SAFE_DROP(viewMtl);
	SAFE_DROP(diffMtl);
	SAFE_DROP(specMtl);
}

Material* MaterialManager::GetDefaultFlatMtl()
{
	_Assert(NULL != flatMtl);
	return flatMtl;
}

Material* MaterialManager::GetDefaultViewMtl()
{
	_Assert(NULL != viewMtl);
	return viewMtl;
}

Material* MaterialManager::GetDefaultDiffMtl()
{
	_Assert(NULL != diffMtl);
	return diffMtl;
}

Material* MaterialManager::GetDefaultSpecMtl()
{
	_Assert(NULL != specMtl);
	return specMtl;
}

std::list<Material*> MaterialManager::GetMaterialList()
{
	return materialList;
}
