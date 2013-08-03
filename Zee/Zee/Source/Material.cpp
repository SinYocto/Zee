#include "Material.h"

void Material::SetTexture(int layerIx, wchar_t* texFileName)	// TODO:不应该直接从文件中CreateTexture再Set, 增加TextureManager类
{
	if(layerIx < 0 || layerIx >= MAX_MATERIAL_TEXTURE_LAYERS)
		return;

	if(mTextureLayer[layerIx])
		SAFE_RELEASE(mTextureLayer[layerIx]);

	D3DXCreateTextureFromFile(gD3DDevice, texFileName, &mTextureLayer[layerIx]);		
}

IDirect3DTexture9* Material::GetTexture(int layerIx)
{
	if(layerIx < 0 || layerIx >= MAX_MATERIAL_TEXTURE_LAYERS)
		return NULL;

	return mTextureLayer[layerIx];
}

D3DXMATRIX Material::UVTransformMatrix()
{
	D3DXMATRIX matScale, matTrans;
	D3DXMatrixScaling(&matScale, mTilesU, mTilesV, 1.0f);
	D3DXMatrixTranslation(&matTrans, mOffsetU, mOffsetV, 0);

	return matScale * matTrans;
}

void Material::SetShader(ShadingMethod shadingMethod)
{
	SAFE_DELETE(mShader);

	switch(shadingMethod)
	{
	case Diffuse:
		mShader = new DiffuseShader(this);
		break;
	case Specular:
		mShader = new SpecularShader(this);
		break;;
	case BumpSpecular:
		mShader = new BumpSpecularShader(this);
		break;
	}

	_Assert(NULL != mShader);
}

void Material::SetAmbientColor(D3DXCOLOR color)
{
	mAmbientColor = color;
}

D3DXCOLOR Material::GetAmbientColor()
{
	return mAmbientColor;
}

void Material::SetDiffuseColor(D3DXCOLOR color)
{
	mDiffuseColor = color;
}

D3DXCOLOR Material::GetDiffuseColor()
{
	return mDiffuseColor;
}

void Material::SetSpecularColor(D3DXCOLOR color)
{
	mSpecularColor = color;
}

D3DXCOLOR Material::GetSpecularColor()
{
	return mSpecularColor;
}

D3DXCOLOR Material::GetFinalSpecularColor()
{
	return mShiness * mSpecularColor;
}

void Material::SetSpecShiness(float shiness)
{
	mShiness = shiness;
}

float Material::GetSpecShiness()
{
	return mShiness;
}

void Material::SetSpecGloss(float gloss)
{
	mGloss = gloss;
}

float Material::GetSpecGloss()
{
	return mGloss;
}

void Material::SetUVTiles(float tilesU, float tilesV)
{
	mTilesU = tilesU;
	mTilesV = tilesV;
}

void Material::SetUVOffset(float offsetU, float offsetV)
{
	mOffsetU = offsetU;
	mOffsetV = offsetV;
}

void Material::Render(Object* object, Geometry* geo, Camera* camera)
{
	_Assert(NULL != mShader);
	_Assert(NULL != geo);
	_Assert(NULL != camera);

	mShader->Render(object, geo, camera);
}

wchar_t* Material::GetName()
{
	return mName;
}

void Material::SetID(DWORD id)
{
	mID = id;
}
