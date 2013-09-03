#include "MaterialManager.h"

Material* MaterialManager::flatMtl = NULL;
Material* MaterialManager::viewMtl = NULL;
Material* MaterialManager::diffMtl = NULL;
Material* MaterialManager::specMtl = NULL;

std::list<Material*> MaterialManager::resourceList;
DWORD MaterialManager::curID = 0;

void MaterialManager::AddMaterial(Material* material)
{
	_Assert(NULL != material);

	material->SetID(curID++);			// QUESTION:ID一直加不会溢出吧
	resourceList.push_back(material);
}

void MaterialManager::Destroy()
{
	for(std::list<Material*>::iterator iter = resourceList.begin(); iter != resourceList.end(); ++iter)
	{
		SAFE_DROP(*iter);		// 这里使用了drop方法而不是delete, 当有Model持有material对象时, 虽然material对象不在GeometryManager中管理了
								// 但Model中仍能正常持有 
	}

	resourceList.clear();

	deleteDefaultMtls();
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
	UtilityShader::CreateEffectPool();
	UtilityShader::CreateEffectFromFile(TEXT("./Source/Shaders/Utility.fx"));	
	_Assert(NULL != UtilityShader::effect);

	FlatShader::CreateEffectFromFile(TEXT("./Source/Shaders/Flat.fx"));	
	_Assert(NULL != FlatShader::mEffect);

	ViewShader::CreateEffectFromFile(TEXT("./Source/Shaders/View.fx"));	
	_Assert(NULL != ViewShader::mEffect);

	DiffuseShader::CreateEffectFromFile(TEXT("./Source/Shaders/Diffuse.fx"));	// time used: 124ms
	_Assert(NULL != DiffuseShader::mEffect);

	SpecularShader::CreateEffectFromFile(TEXT("./Source/Shaders/Specular.fx"));		// time used: 230ms
	_Assert(NULL != SpecularShader::mEffect);

	BumpSpecularShader::CreateEffectFromFile(TEXT("./Source/Shaders/BumpSpecular.fx"));		// time used: 248ms
	_Assert(NULL != BumpSpecularShader::mEffect);

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
	flatMtl = new Material(L"defaultFlatMtl");
	flatMtl->SetShader(Flat);
	flatMtl->SetID(curID++);

	viewMtl = new Material(L"defaultViewMtl");
	viewMtl->SetShader(View);
	viewMtl->SetID(curID++);

	diffMtl = new Material(L"defaultDiffMtl");
	diffMtl->SetShader(Diffuse);
	diffMtl->SetID(curID++);

	specMtl = new Material(L"defaultSpecMtl");
	specMtl->SetShader(Specular);
	specMtl->SetID(curID++);
}

void MaterialManager::deleteDefaultMtls()
{
	SAFE_DROP(flatMtl);
	SAFE_DROP(viewMtl);
	SAFE_DROP(diffMtl);
	SAFE_DROP(specMtl);
}
