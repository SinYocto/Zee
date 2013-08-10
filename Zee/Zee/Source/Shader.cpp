#include "Shader.h"
#include "Material.h"
#include "Geometry.h"
#include "Camera.h"
#include "Object.h"
#include "LightManager.h"

// DiffuseShader
LPD3DXEFFECTPOOL UtilityShader::pool = NULL;
LPD3DXEFFECT UtilityShader::effect = NULL;

LPD3DXEFFECT DiffuseShader::mEffect = NULL;
LPD3DXEFFECT SpecularShader::mEffect = NULL;
LPD3DXEFFECT BumpSpecularShader::mEffect = NULL;

void DiffuseShader::SetColorTex(wchar_t* texFileName)
{
	mMaterial->SetTexture(0, texFileName);
}

void DiffuseShader::SetAmbientColor(D3DXCOLOR color)
{
	mMaterial->SetAmbientColor(color);
}

void DiffuseShader::SetDiffuseColor(D3DXCOLOR color)
{
	mMaterial->SetDiffuseColor(color);
}

void DiffuseShader::SetUVTiles(float tilesU, float tilesV)
{
	mMaterial->SetUVTiles(tilesU, tilesV);
}

void DiffuseShader::SetUVOffset(float offsetU, float offsetV)
{
	mMaterial->SetUVOffset(offsetU, offsetV);
}

void DiffuseShader::Render(Object* object, Geometry* geo, Camera* camera)
{
	_Assert(NULL != geo);
	_Assert(NULL != camera);

	geo->SetVertexStream();
	geo->SetVertexDeclaration();

	DiffuseShader::mEffect->SetTechnique("Diffuse");

	DiffuseShader::mEffect->SetMatrix("matWVP", &(object->LocalToWorldMatrix()*camera->ViewMatrix()*camera->ProjMatrix()));
	DiffuseShader::mEffect->SetMatrix("matWorld", &(object->LocalToWorldMatrix()));
	DiffuseShader::mEffect->SetMatrix("matUVTransform", &(mMaterial->UVTransformMatrix()));

	if(NULL == mMaterial->GetTexture(0))
	{
		DiffuseShader::mEffect->SetBool("useColorTex", false);
	}
	else
	{
		DiffuseShader::mEffect->SetBool("useColorTex", true);	
		DiffuseShader::mEffect->SetTexture("colorTex", mMaterial->GetTexture(0));
	}

	DiffuseShader::mEffect->SetRawValue("mtlAmbient", &(mMaterial->GetAmbientColor()), 0, sizeof(D3DXCOLOR));
	DiffuseShader::mEffect->SetRawValue("mtlDiffuse", &(mMaterial->GetDiffuseColor()), 0, sizeof(D3DXCOLOR));

	DiffuseShader::mEffect->Begin(0, 0);
	DiffuseShader::mEffect->BeginPass(0);

	geo->Draw();

	DiffuseShader::mEffect->EndPass();
	DiffuseShader::mEffect->End();
}

// SpecularShader
void SpecularShader::SetColorTex(wchar_t* texFileName)
{
	mMaterial->SetTexture(0, texFileName);
}

void SpecularShader::SetAmbientColor(D3DXCOLOR color)
{
	mMaterial->SetAmbientColor(color);
}


void SpecularShader::SetDiffuseColor(D3DXCOLOR color)
{
	mMaterial->SetDiffuseColor(color);
}

void SpecularShader::SetSpecularColor(D3DXCOLOR color)
{
	mMaterial->SetSpecularColor(color);
}

void SpecularShader::SetSpecShiness(float _shiness)
{
	mMaterial->SetSpecShiness(_shiness);
}

void SpecularShader::SetSpecGloss(float _gloss)
{
	mMaterial->SetSpecGloss(_gloss);
}

void SpecularShader::SetUVTiles(float tilesU, float tilesV)
{
	mMaterial->SetUVTiles(tilesU, tilesV);
}

void SpecularShader::SetUVOffset(float offsetU, float offsetV)
{
	mMaterial->SetUVOffset(offsetU, offsetV);
}

void SpecularShader::Render(Object* object, Geometry* geo, Camera* camera)
{
	_Assert(NULL != geo);
	_Assert(NULL != camera);

	geo->SetVertexStream();
	geo->SetVertexDeclaration();

	SpecularShader::mEffect->SetTechnique("Specular");

	SpecularShader::mEffect->SetMatrix("matWVP", &(object->LocalToWorldMatrix()*camera->ViewMatrix()*camera->ProjMatrix()));
	SpecularShader::mEffect->SetMatrix("matWorld", &(object->LocalToWorldMatrix()));
	SpecularShader::mEffect->SetMatrix("matUVTransform", &(mMaterial->UVTransformMatrix()));

	SpecularShader::mEffect->SetRawValue("eyePos", &(camera->GetRelativePosition()), 0, sizeof(Vector3));

	if(NULL == mMaterial->GetTexture(0))
	{
		SpecularShader::mEffect->SetBool("useColorTex", false);
	}
	else
	{
		SpecularShader::mEffect->SetBool("useColorTex", true);	
		SpecularShader::mEffect->SetTexture("colorTex", mMaterial->GetTexture(0));
	}

	SpecularShader::mEffect->SetRawValue("mtlAmbient", &(mMaterial->GetAmbientColor()), 0, sizeof(D3DXCOLOR));
	SpecularShader::mEffect->SetRawValue("mtlDiffuse", &(mMaterial->GetDiffuseColor()), 0, sizeof(D3DXCOLOR));
	SpecularShader::mEffect->SetRawValue("mtlSpec", &(mMaterial->GetFinalSpecularColor()), 0, sizeof(D3DXCOLOR));
	SpecularShader::mEffect->SetFloat("gloss", mMaterial->GetSpecGloss());

	SpecularShader::mEffect->Begin(0, 0);
	SpecularShader::mEffect->BeginPass(0);

	geo->Draw();

	SpecularShader::mEffect->EndPass();
	SpecularShader::mEffect->End();
}

// BumpSpecularShader
void BumpSpecularShader::SetColorTex(wchar_t* texFileName)
{
	mMaterial->SetTexture(0, texFileName);
}

void BumpSpecularShader::SetNormalTex(wchar_t* texFileName)
{
	mMaterial->SetTexture(1, texFileName);
}

void BumpSpecularShader::SetAmbientColor(D3DXCOLOR color)
{
	mMaterial->SetAmbientColor(color);
}

void BumpSpecularShader::SetDiffuseColor(D3DXCOLOR color)
{
	mMaterial->SetDiffuseColor(color);
}

void BumpSpecularShader::SetSpecularColor(D3DXCOLOR color)
{
	mMaterial->SetSpecularColor(color);
}

void BumpSpecularShader::SetSpecShiness(float shiness)
{
	mMaterial->SetSpecShiness(shiness);
}

void BumpSpecularShader::SetSpecGloss(float gloss)
{
	mMaterial->SetSpecGloss(gloss);
}

void BumpSpecularShader::SetUVTiles(float tilesU, float tilesV)
{
	mMaterial->SetUVTiles(tilesU, tilesV);
}

void BumpSpecularShader::SetUVOffset(float offsetU, float offsetV)
{
	mMaterial->SetUVOffset(offsetU, offsetV);
}

void BumpSpecularShader::Render(Object* object, Geometry* geo, Camera* camera)
{
	_Assert(NULL != geo);
	_Assert(NULL != camera);

	geo->SetVertexStream();
	geo->SetVertexDeclaration();

	BumpSpecularShader::mEffect->SetTechnique("BumpSpecular");

	BumpSpecularShader::mEffect->SetMatrix("matWVP", &(object->LocalToWorldMatrix()*camera->ViewMatrix()*camera->ProjMatrix()));
	BumpSpecularShader::mEffect->SetMatrix("matWorld", &(object->LocalToWorldMatrix()));
	BumpSpecularShader::mEffect->SetMatrix("matUVTransform", &(mMaterial->UVTransformMatrix()));

	BumpSpecularShader::mEffect->SetRawValue("eyePos", &(camera->GetRelativePosition()), 0, sizeof(Vector3));

	if(NULL == mMaterial->GetTexture(0))
	{
		BumpSpecularShader::mEffect->SetBool("useColorTex", false);
	}
	else
	{
		BumpSpecularShader::mEffect->SetBool("useColorTex", true);	
		BumpSpecularShader::mEffect->SetTexture("colorTex", mMaterial->GetTexture(0));
	}

	BumpSpecularShader::mEffect->SetTexture("normalTex", mMaterial->GetTexture(1));

	BumpSpecularShader::mEffect->SetRawValue("mtlAmbient", &(mMaterial->GetAmbientColor()), 0, sizeof(D3DXCOLOR));
	BumpSpecularShader::mEffect->SetRawValue("mtlDiffuse", &(mMaterial->GetDiffuseColor()), 0, sizeof(D3DXCOLOR));
	BumpSpecularShader::mEffect->SetRawValue("mtlSpec", &(mMaterial->GetFinalSpecularColor()), 0, sizeof(D3DXCOLOR));
	BumpSpecularShader::mEffect->SetFloat("gloss", mMaterial->GetSpecGloss());

	BumpSpecularShader::mEffect->Begin(0, 0);
	BumpSpecularShader::mEffect->BeginPass(0);

	geo->Draw();

	BumpSpecularShader::mEffect->EndPass();
	BumpSpecularShader::mEffect->End();
}

void UtilityShader::SetupSharedParams()
{
	_Assert(NULL != effect);

	effect->SetRawValue("ambientLight", &(LightManager::GetFinalAmbientColor()), 0 , sizeof(Vector3));
	effect->SetRawValue("directionalLights", LightManager::directionalLightsData, 0, sizeof(LightManager::directionalLightsData));
	effect->SetRawValue("pointLights", LightManager::pointLightsData, 0, sizeof(LightManager::pointLightsData));
}
