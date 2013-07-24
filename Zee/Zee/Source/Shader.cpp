#include "Shader.h"
#include "Material.h"
#include "Camera.h"
#include "Object.h"
#include "Geometry.h"
#include "AmbientLight.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "LightManager.h"

// DiffuseShader
LPD3DXEFFECT DiffuseShader::effect = NULL;

void DiffuseShader::SetColorTex(wchar_t* texFileName)
{
	material->SetTexture(0, texFileName);
}

void DiffuseShader::SetAmbientColor(D3DXCOLOR color)
{
	material->SetAmbientColor(color);
}

void DiffuseShader::SetDiffuseColor(D3DXCOLOR color)
{
	material->SetDiffuseColor(color);
}

void DiffuseShader::SetUVTiles(float _tilesU, float _tilesV)
{
	material->SetUVTiles(_tilesU, _tilesV);
}

void DiffuseShader::SetUVOffset(float _offsetU, float _offsetV)
{
	material->SetUVOffset(_offsetU, _offsetV);
}

void DiffuseShader::Render(Object* object, Geometry* geo, Camera* camera)
{
	_Assert(NULL != geo);
	_Assert(NULL != camera);

	geo->SetVertexStream();
	geo->SetVertexDeclaration();

	DiffuseShader::effect->SetTechnique("Diffuse");

	DiffuseShader::effect->SetMatrix("matWVP", &(object->LocalToWorldMatrix()*camera->ViewMatrix()*camera->ProjMatrix()));
	DiffuseShader::effect->SetMatrix("matWorld", &(object->LocalToWorldMatrix()));
	DiffuseShader::effect->SetMatrix("matUVTransform", &(material->UVTransformMatrix()));

	DiffuseShader::effect->SetRawValue("ambientLight", &(LightManager::GetFinalAmbientColor()), 0 , sizeof(Vector3));
	DiffuseShader::effect->SetRawValue("directionalLights", LightManager::directionalLightsData, 
		0, sizeof(LightManager::directionalLightsData));
	DiffuseShader::effect->SetRawValue("pointLights", LightManager::pointLightsData, 0, sizeof(LightManager::pointLightsData));

	if(NULL == material->GetTexture(0))
	{
		DiffuseShader::effect->SetBool("useColorTex", false);
	}
	else
	{
		DiffuseShader::effect->SetBool("useColorTex", true);	
		DiffuseShader::effect->SetTexture("colorTex", material->GetTexture(0));
	}

	DiffuseShader::effect->SetRawValue("mtlAmbient", &(material->GetAmbientColor()), 0, sizeof(D3DXCOLOR));
	DiffuseShader::effect->SetRawValue("mtlDiffuse", &(material->GetDiffuseColor()), 0, sizeof(D3DXCOLOR));

	DiffuseShader::effect->Begin(0, 0);
	DiffuseShader::effect->BeginPass(0);

	geo->Draw();

	DiffuseShader::effect->EndPass();
	DiffuseShader::effect->End();
}

// SpecularShader
LPD3DXEFFECT SpecularShader::effect = NULL;

void SpecularShader::SetColorTex(wchar_t* texFileName)
{
	material->SetTexture(0, texFileName);
}

void SpecularShader::SetAmbientColor(D3DXCOLOR color)
{
	material->SetAmbientColor(color);
}


void SpecularShader::SetDiffuseColor(D3DXCOLOR color)
{
	material->SetDiffuseColor(color);
}

void SpecularShader::SetSpecularColor(D3DXCOLOR color)
{
	material->SetSpecularColor(color);
}

void SpecularShader::SetSpecShiness(float _shiness)
{
	material->SetSpecShiness(_shiness);
}

void SpecularShader::SetSpecGloss(float _gloss)
{
	material->SetSpecGloss(_gloss);
}

void SpecularShader::SetUVTiles(float _tilesU, float _tilesV)
{
	material->SetUVTiles(_tilesU, _tilesV);
}

void SpecularShader::SetUVOffset(float _offsetU, float _offsetV)
{
	material->SetUVOffset(_offsetU, _offsetV);
}

void SpecularShader::Render(Object* object, Geometry* geo, Camera* camera)
{
	_Assert(NULL != geo);
	_Assert(NULL != camera);

	geo->SetVertexStream();
	geo->SetVertexDeclaration();

	SpecularShader::effect->SetTechnique("Specular");

	SpecularShader::effect->SetMatrix("matWVP", &(object->LocalToWorldMatrix()*camera->ViewMatrix()*camera->ProjMatrix()));
	SpecularShader::effect->SetMatrix("matWorld", &(object->LocalToWorldMatrix()));
	SpecularShader::effect->SetMatrix("matUVTransform", &(material->UVTransformMatrix()));

	SpecularShader::effect->SetRawValue("eyePos", &(camera->GetRelativePosition()), 0, sizeof(Vector3));

	SpecularShader::effect->SetRawValue("ambientLight", &(LightManager::GetFinalAmbientColor()), 0 , sizeof(Vector3));
	SpecularShader::effect->SetRawValue("directionalLights", LightManager::directionalLightsData, 0, sizeof(LightManager::directionalLightsData));
	SpecularShader::effect->SetRawValue("pointLights", LightManager::pointLightsData, 0, sizeof(LightManager::pointLightsData));

	if(NULL == material->GetTexture(0))
	{
		SpecularShader::effect->SetBool("useColorTex", false);
	}
	else
	{
		SpecularShader::effect->SetBool("useColorTex", true);	
		SpecularShader::effect->SetTexture("colorTex", material->GetTexture(0));
	}

	SpecularShader::effect->SetRawValue("mtlAmbient", &(material->GetAmbientColor()), 0, sizeof(D3DXCOLOR));
	SpecularShader::effect->SetRawValue("mtlDiffuse", &(material->GetDiffuseColor()), 0, sizeof(D3DXCOLOR));
	SpecularShader::effect->SetRawValue("mtlSpec", &(material->GetFinalSpecularColor()), 0, sizeof(D3DXCOLOR));
	SpecularShader::effect->SetFloat("gloss", material->GetSpecGloss());

	SpecularShader::effect->Begin(0, 0);
	SpecularShader::effect->BeginPass(0);

	geo->Draw();

	SpecularShader::effect->EndPass();
	SpecularShader::effect->End();
}

// BumpSpecularShader
LPD3DXEFFECT BumpSpecularShader::effect = NULL;

void BumpSpecularShader::SetColorTex(wchar_t* texFileName)
{
	material->SetTexture(0, texFileName);
}

void BumpSpecularShader::SetNormalTex(wchar_t* texFileName)
{
	material->SetTexture(1, texFileName);
}

void BumpSpecularShader::SetAmbientColor(D3DXCOLOR color)
{
	material->SetAmbientColor(color);
}

void BumpSpecularShader::SetDiffuseColor(D3DXCOLOR color)
{
	material->SetDiffuseColor(color);
}

void BumpSpecularShader::SetSpecularColor(D3DXCOLOR color)
{
	material->SetSpecularColor(color);
}

void BumpSpecularShader::SetSpecShiness(float _shiness)
{
	material->SetSpecShiness(_shiness);
}

void BumpSpecularShader::SetSpecGloss(float _gloss)
{
	material->SetSpecGloss(_gloss);
}

void BumpSpecularShader::SetUVTiles(float _tilesU, float _tilesV)
{
	material->SetUVTiles(_tilesU, _tilesV);
}

void BumpSpecularShader::SetUVOffset(float _offsetU, float _offsetV)
{
	material->SetUVOffset(_offsetU, _offsetV);
}

void BumpSpecularShader::Render(Object* object, Geometry* geo, Camera* camera)
{
	_Assert(NULL != geo);
	_Assert(NULL != camera);

	geo->SetVertexStream();
	geo->SetVertexDeclaration();

	BumpSpecularShader::effect->SetTechnique("BumpSpecular");

	BumpSpecularShader::effect->SetMatrix("matWVP", &(object->LocalToWorldMatrix()*camera->ViewMatrix()*camera->ProjMatrix()));
	BumpSpecularShader::effect->SetMatrix("matWorld", &(object->LocalToWorldMatrix()));
	BumpSpecularShader::effect->SetMatrix("matUVTransform", &(material->UVTransformMatrix()));

	BumpSpecularShader::effect->SetRawValue("eyePos", &(camera->GetRelativePosition()), 0, sizeof(Vector3));

	BumpSpecularShader::effect->SetRawValue("ambientLight", &(LightManager::GetFinalAmbientColor()), 0 , sizeof(Vector3));
	BumpSpecularShader::effect->SetRawValue("directionalLights", LightManager::directionalLightsData, 0, sizeof(LightManager::directionalLightsData));
	BumpSpecularShader::effect->SetRawValue("pointLights", LightManager::pointLightsData, 0, sizeof(LightManager::pointLightsData));

	BumpSpecularShader::effect->SetTexture("colorTex", material->GetTexture(0));
	BumpSpecularShader::effect->SetTexture("normalTex", material->GetTexture(1));

	BumpSpecularShader::effect->SetRawValue("mtlAmbient", &(material->GetAmbientColor()), 0, sizeof(D3DXCOLOR));
	BumpSpecularShader::effect->SetRawValue("mtlDiffuse", &(material->GetDiffuseColor()), 0, sizeof(D3DXCOLOR));
	BumpSpecularShader::effect->SetRawValue("mtlSpec", &(material->GetFinalSpecularColor()), 0, sizeof(D3DXCOLOR));
	BumpSpecularShader::effect->SetFloat("gloss", material->GetSpecGloss());

	BumpSpecularShader::effect->Begin(0, 0);
	BumpSpecularShader::effect->BeginPass(0);

	geo->Draw();

	BumpSpecularShader::effect->EndPass();
	BumpSpecularShader::effect->End();
}
