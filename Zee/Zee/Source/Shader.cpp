#include "Shader.h"
#include "Material.h"
#include "Geometry.h"
#include "Camera.h"
#include "Engine.h"

// DiffuseShader
LPD3DXEFFECTPOOL UtilityShader::pool = NULL;
LPD3DXEFFECT UtilityShader::effect = NULL;


LPD3DXEFFECT FlatShader::mEffect = NULL;
LPD3DXEFFECT ViewShader::mEffect = NULL;
LPD3DXEFFECT DiffuseShader::mEffect = NULL;
LPD3DXEFFECT SpecularShader::mEffect = NULL;
LPD3DXEFFECT BumpSpecularShader::mEffect = NULL;

// FlatShader
void FlatShader::SetColorTex(wchar_t* texFileName)
{
	mMaterial->SetTexture(0, texFileName);
}

void FlatShader::SetDiffuseColor(D3DXCOLOR color)
{
	mMaterial->SetDiffuseColor(color);
}

void FlatShader::SetUVTiles(float tilesU, float tilesV)
{
	mMaterial->SetUVTiles(tilesU, tilesV);
}

void FlatShader::SetUVOffset(float offsetU, float offsetV)
{
	mMaterial->SetUVOffset(offsetU, offsetV);
}

void FlatShader::Render(const D3DXMATRIX& matWorld, Geometry* geo, Camera* camera, bool isStandAlone)
{
	_Assert(NULL != geo);
	_Assert(NULL != camera);

	geo->SetVertexStream();
	geo->SetVertexDeclaration();

	FlatShader::mEffect->SetTechnique("Flat");

	FlatShader::mEffect->SetMatrix("matWVP", &(matWorld * camera->ViewMatrix() * camera->ProjMatrix()));
	FlatShader::mEffect->SetMatrix("matUVTransform", &matWorld);

	if(NULL == mMaterial->GetTexture(0))
	{
		FlatShader::mEffect->SetBool("useColorTex", false);
	}
	else
	{
		FlatShader::mEffect->SetBool("useColorTex", true);	
		FlatShader::mEffect->SetTexture("colorTex", mMaterial->GetTexture(0)->GetD3DTexture());
	}

	FlatShader::mEffect->SetRawValue("mtlColor", &(mMaterial->GetDiffuseColor()), 0, sizeof(D3DXCOLOR));

	FlatShader::mEffect->CommitChanges();

	if(isStandAlone)
	{
		FlatShader::mEffect->Begin(0, 0);
		FlatShader::mEffect->BeginPass(0);
	}

	geo->Draw();

	if(isStandAlone)
	{
		FlatShader::mEffect->EndPass();
		FlatShader::mEffect->End();
	}
}

void FlatShader::OnLostDevice()
{
	mEffect->OnLostDevice();
}

void FlatShader::OnResetDevice()
{
	mEffect->OnResetDevice();
}

void FlatShader::CreateEffectFromFile( wchar_t* FXFileName )
{
	_Assert(NULL != UtilityShader::pool);
	D3DXCreateEffectFromFile(gEngine->GetDriver()->GetD3DDevice(), FXFileName, NULL, NULL, D3DXSHADER_DEBUG, UtilityShader::pool, &mEffect, NULL);

	_Assert(NULL != mEffect);
}

// ViewShader
void ViewShader::SetColorTex(wchar_t* texFileName)
{
	mMaterial->SetTexture(0, texFileName);
}

void ViewShader::SetDiffuseColor(D3DXCOLOR color)
{
	mMaterial->SetDiffuseColor(color);
}

void ViewShader::SetUVTiles(float tilesU, float tilesV)
{
	mMaterial->SetUVTiles(tilesU, tilesV);
}

void ViewShader::SetUVOffset(float offsetU, float offsetV)
{
	mMaterial->SetUVOffset(offsetU, offsetV);
}

void ViewShader::Render(const D3DXMATRIX& matWorld, Geometry* geo, Camera* camera, bool isStandAlone)
{
	_Assert(NULL != geo);
	_Assert(NULL != camera);

	geo->SetVertexStream();
	geo->SetVertexDeclaration();

	ViewShader::mEffect->SetTechnique("View");

	ViewShader::mEffect->SetMatrix("matWVP", &(matWorld * camera->ViewMatrix() * camera->ProjMatrix()));
	ViewShader::mEffect->SetMatrix("matWorld", &matWorld);
	ViewShader::mEffect->SetMatrix("matUVTransform", &(mMaterial->UVTransformMatrix()));

	ViewShader::mEffect->SetRawValue("eyePos", &(camera->GetRelativePosition()), 0, sizeof(Vector3));

	if(NULL == mMaterial->GetTexture(0))
	{
		ViewShader::mEffect->SetBool("useColorTex", false);
	}
	else
	{
		ViewShader::mEffect->SetBool("useColorTex", true);	
		ViewShader::mEffect->SetTexture("colorTex", mMaterial->GetTexture(0)->GetD3DTexture());
	}

	ViewShader::mEffect->SetRawValue("mtlColor", &(mMaterial->GetDiffuseColor()), 0, sizeof(D3DXCOLOR));

	ViewShader::mEffect->CommitChanges();

	if(isStandAlone)
	{
		ViewShader::mEffect->Begin(0, 0);
		ViewShader::mEffect->BeginPass(0);
	}

	geo->Draw();

	if(isStandAlone)
	{
		ViewShader::mEffect->EndPass();
		ViewShader::mEffect->End();
	}
}

void ViewShader::OnLostDevice()
{
	mEffect->OnLostDevice();
}

void ViewShader::OnResetDevice()
{
	mEffect->OnResetDevice();
}

void ViewShader::CreateEffectFromFile( wchar_t* FXFileName )
{
	_Assert(NULL != UtilityShader::pool);
	D3DXCreateEffectFromFile(gEngine->GetDriver()->GetD3DDevice(), FXFileName, NULL, NULL, D3DXSHADER_DEBUG, UtilityShader::pool, &mEffect, NULL);

	_Assert(NULL != mEffect);
}

// DiffuseShader
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

void DiffuseShader::Render(const D3DXMATRIX& matWorld, Geometry* geo, Camera* camera, bool isStandAlone)
{
	_Assert(NULL != geo);
	_Assert(NULL != camera);

	geo->SetVertexStream();
	geo->SetVertexDeclaration();

	DiffuseShader::mEffect->SetTechnique("Diffuse");

	DiffuseShader::mEffect->SetMatrix("matWVP", &(matWorld * camera->ViewMatrix() * camera->ProjMatrix()));
	DiffuseShader::mEffect->SetMatrix("matWorld", &matWorld);
	DiffuseShader::mEffect->SetMatrix("matUVTransform", &(mMaterial->UVTransformMatrix()));

	if(NULL == mMaterial->GetTexture(0))
	{
		DiffuseShader::mEffect->SetBool("useColorTex", false);
	}
	else
	{
		DiffuseShader::mEffect->SetBool("useColorTex", true);	
		DiffuseShader::mEffect->SetTexture("colorTex", mMaterial->GetTexture(0)->GetD3DTexture());
	}

	DiffuseShader::mEffect->SetRawValue("mtlAmbient", &(mMaterial->GetAmbientColor()), 0, sizeof(D3DXCOLOR));
	DiffuseShader::mEffect->SetRawValue("mtlDiffuse", &(mMaterial->GetDiffuseColor()), 0, sizeof(D3DXCOLOR));

	DiffuseShader::mEffect->CommitChanges();

	if(isStandAlone)
	{
		DiffuseShader::mEffect->Begin(0, 0);
		DiffuseShader::mEffect->BeginPass(0);
	}

	geo->Draw();
	
	if(isStandAlone)
	{
		DiffuseShader::mEffect->EndPass();
		DiffuseShader::mEffect->End();
	}
}

void DiffuseShader::OnLostDevice()
{
	mEffect->OnLostDevice();
}

void DiffuseShader::OnResetDevice()
{
	mEffect->OnResetDevice();
}

void DiffuseShader::CreateEffectFromFile( wchar_t* FXFileName )
{
	_Assert(NULL != UtilityShader::pool);
	D3DXCreateEffectFromFile(gEngine->GetDriver()->GetD3DDevice(), FXFileName, NULL, NULL, D3DXSHADER_DEBUG, UtilityShader::pool, &mEffect, NULL);

	_Assert(NULL != mEffect);
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

void SpecularShader::Render(const D3DXMATRIX& matWorld, Geometry* geo, Camera* camera, bool isStandAlone)
{
	_Assert(NULL != geo);
	_Assert(NULL != camera);

	geo->SetVertexStream();
	geo->SetVertexDeclaration();

	SpecularShader::mEffect->SetTechnique("Specular");

	SpecularShader::mEffect->SetMatrix("matWVP", &(matWorld * camera->ViewMatrix() * camera->ProjMatrix()));
	SpecularShader::mEffect->SetMatrix("matWorld", &matWorld);
	SpecularShader::mEffect->SetMatrix("matUVTransform", &(mMaterial->UVTransformMatrix()));

	SpecularShader::mEffect->SetRawValue("eyePos", &(camera->GetRelativePosition()), 0, sizeof(Vector3));

	if(NULL == mMaterial->GetTexture(0))
	{
		SpecularShader::mEffect->SetBool("useColorTex", false);
	}
	else
	{
		SpecularShader::mEffect->SetBool("useColorTex", true);	
		SpecularShader::mEffect->SetTexture("colorTex", mMaterial->GetTexture(0)->GetD3DTexture());
	}

	SpecularShader::mEffect->SetRawValue("mtlAmbient", &(mMaterial->GetAmbientColor()), 0, sizeof(D3DXCOLOR));
	SpecularShader::mEffect->SetRawValue("mtlDiffuse", &(mMaterial->GetDiffuseColor()), 0, sizeof(D3DXCOLOR));
	SpecularShader::mEffect->SetRawValue("mtlSpec", &(mMaterial->GetFinalSpecularColor()), 0, sizeof(D3DXCOLOR));
	SpecularShader::mEffect->SetFloat("gloss", mMaterial->GetSpecGloss());

	SpecularShader::mEffect->CommitChanges();

	if(isStandAlone)
	{
		SpecularShader::mEffect->Begin(0, 0);
		SpecularShader::mEffect->BeginPass(0);
	}

	geo->Draw();

	if(isStandAlone)
	{
		SpecularShader::mEffect->EndPass();
		SpecularShader::mEffect->End();
	}
}

void SpecularShader::OnLostDevice()
{
	mEffect->OnLostDevice();
}

void SpecularShader::OnResetDevice()
{
	mEffect->OnResetDevice();
}

void SpecularShader::CreateEffectFromFile( wchar_t* FXFileName )
{
	_Assert(NULL != UtilityShader::pool);
	D3DXCreateEffectFromFile(gEngine->GetDriver()->GetD3DDevice(), FXFileName, NULL, NULL, D3DXSHADER_DEBUG, UtilityShader::pool, &mEffect, NULL);

	_Assert(NULL != mEffect);
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

void BumpSpecularShader::Render(const D3DXMATRIX& matWorld, Geometry* geo, Camera* camera, bool isStandAlone)
{
	_Assert(NULL != geo);
	_Assert(NULL != camera);

	geo->SetVertexStream();
	geo->SetVertexDeclaration();

	BumpSpecularShader::mEffect->SetTechnique("BumpSpecular");

	BumpSpecularShader::mEffect->SetMatrix("matWVP", &(matWorld * camera->ViewMatrix() * camera->ProjMatrix()));
	BumpSpecularShader::mEffect->SetMatrix("matWorld", &matWorld);
	BumpSpecularShader::mEffect->SetMatrix("matUVTransform", &(mMaterial->UVTransformMatrix()));

	BumpSpecularShader::mEffect->SetRawValue("eyePos", &(camera->GetRelativePosition()), 0, sizeof(Vector3));

	if(NULL == mMaterial->GetTexture(0))
	{
		BumpSpecularShader::mEffect->SetBool("useColorTex", false);
	}
	else
	{
		BumpSpecularShader::mEffect->SetBool("useColorTex", true);	
		BumpSpecularShader::mEffect->SetTexture("colorTex", mMaterial->GetTexture(0)->GetD3DTexture());
	}

	BumpSpecularShader::mEffect->SetTexture("normalTex", mMaterial->GetTexture(1)->GetD3DTexture());

	BumpSpecularShader::mEffect->SetRawValue("mtlAmbient", &(mMaterial->GetAmbientColor()), 0, sizeof(D3DXCOLOR));
	BumpSpecularShader::mEffect->SetRawValue("mtlDiffuse", &(mMaterial->GetDiffuseColor()), 0, sizeof(D3DXCOLOR));
	BumpSpecularShader::mEffect->SetRawValue("mtlSpec", &(mMaterial->GetFinalSpecularColor()), 0, sizeof(D3DXCOLOR));
	BumpSpecularShader::mEffect->SetFloat("gloss", mMaterial->GetSpecGloss());

	BumpSpecularShader::mEffect->CommitChanges();

	if(isStandAlone)
	{
		BumpSpecularShader::mEffect->Begin(0, 0);
		BumpSpecularShader::mEffect->BeginPass(0);
	}

	geo->Draw();

	if(isStandAlone)
	{
		BumpSpecularShader::mEffect->EndPass();
		BumpSpecularShader::mEffect->End();
	}
}

void BumpSpecularShader::OnLostDevice()
{
	mEffect->OnLostDevice();
}

void BumpSpecularShader::OnResetDevice()
{
	mEffect->OnResetDevice();
}

void BumpSpecularShader::CreateEffectFromFile( wchar_t* FXFileName )
{
	_Assert(NULL != UtilityShader::pool);
	D3DXCreateEffectFromFile(gEngine->GetDriver()->GetD3DDevice(), FXFileName, NULL, NULL, D3DXSHADER_DEBUG, UtilityShader::pool, &mEffect, NULL);

	_Assert(NULL != mEffect);
}

void UtilityShader::SetupSharedParams()
{
	_Assert(NULL != effect);

	LightManager* lightMgr = gEngine->GetLightManager();

	effect->SetRawValue("ambientLight", &(lightMgr->GetFinalAmbientColor()), 0 , sizeof(Vector3));

	effect->SetRawValue("directionalLights", lightMgr->GetDirectionalLightsData(), 0, 
		MAX_NUM_DIRECTIONAL_LIGHTS * sizeof(DirectionalLightData));

	effect->SetRawValue("pointLights", lightMgr->GetPointLightsData(), 0, 
		MAX_NUM_POINT_LIGHTS * sizeof(PointLightData));
}

void UtilityShader::OnLostDevice()
{
	effect->OnLostDevice();
}

void UtilityShader::OnResetDevice()
{
	effect->OnResetDevice();
}

void UtilityShader::CreateEffectPool()
{
	HRESULT hr = D3DXCreateEffectPool(&pool);
	_Assert(SUCCEEDED(hr));
}

void UtilityShader::CreateEffectFromFile( wchar_t* FXFileName )
{
	_Assert(NULL != pool);
	D3DXCreateEffectFromFile(gEngine->GetDriver()->GetD3DDevice(), FXFileName, NULL, NULL, D3DXSHADER_DEBUG, pool, &effect, NULL);

	_Assert(NULL != effect);
}
