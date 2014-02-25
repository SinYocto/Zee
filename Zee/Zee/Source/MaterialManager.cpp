#include "MaterialManager.h"
#include "Engine.h"

MaterialManager::MaterialManager()
{

}

void MaterialManager::AddMaterial(Material* material)	// 注意没有grab, 每增加mtl的引用计数(是否不妥?)
{
	_Assert(NULL != material);

	if(YString::isEmpty(material->GetFilePath()))
	{
		wchar_t mtlName[MAX_STR_LEN];
		YString::Copy(mtlName, _countof(mtlName), material->GetName());
		YString::Format(mtlName, L"%s_%d", mtlName, material->GetID());
		material->SetName(mtlName);

		material->SaveToFile(L"Assets/Materials");
	}

	_Assert(!YString::isEmpty(material->GetFilePath()));
	MtlHashMap::iterator iter = mMaterials.find(material->GetFilePath());
	if(iter != mMaterials.end())
	{
		Log(L"warning: MaterialManager::AddMaterial() mtl already exists!\n");
		return;
	}
	else
	{
		mMaterials[material->GetFilePath()] = material;
	}
}

void MaterialManager::Destroy()
{
	for(MtlHashMap::iterator iter = mMaterials.begin(); iter != mMaterials.end(); ++iter)
	{
		SAFE_DROP(iter->second);
	}

	mMaterials.clear();
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

Material* MaterialManager::GetDefaultFlatMtl()
{
	return GetOrCreateMaterial(L"Assets/Materials/defaultFlat.material");
}

Material* MaterialManager::GetDefaultViewMtl()
{
	return GetOrCreateMaterial(L"Assets/Materials/defaultView.material");
}

Material* MaterialManager::GetDefaultDiffMtl()
{
	return GetOrCreateMaterial(L"Assets/Materials/defaultDiff.material");
}

Material* MaterialManager::GetDefaultSpecMtl()
{
	return GetOrCreateMaterial(L"Assets/Materials/defaultSpec.material");
}

MtlHashMap MaterialManager::GetMtlHashMap()
{
	return mMaterials;
}

Material* MaterialManager::GetOrCreateMaterial(const wchar_t* filePath)
{
	Material* resultMtl = NULL;

	MtlHashMap::iterator iter = mMaterials.find(filePath);
	if(iter != mMaterials.end())
	{
		resultMtl = iter->second;
		_Assert(NULL != resultMtl);
	}
	else
	{
		_Assert(YFile::Exist(filePath));

		wchar_t fileName[MAX_STR_LEN];
		YString::GetFileName(fileName, _countof(fileName), filePath, false);

		resultMtl = New Material(fileName, filePath);

		mMaterials[filePath] = resultMtl;
	}

	return resultMtl;
}
