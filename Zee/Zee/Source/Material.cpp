#include "Material.h"
#include "Engine.h"
#include "IDAllocator.h"

Material::Material( const wchar_t* name ) 
:mID(IDAllocator::ID_ANY)
,mShader(NULL)
,mShadingMethod(InvalidMethod)
,mAmbientColor(D3DXCOLOR_WHITE)
,mDiffuseColor(D3DXCOLOR_WHITE)
,mSpecularColor(D3DXCOLOR_WHITE)
,mShiness(1.0f)
,mGloss(50.0f)
,mTilesU(1.0f)
,mTilesV(1.0f)
,mOffsetU(0)
,mOffsetV(0)
{
	YString::Copy(mName, _countof(mName), name);

	for(int layerIx = 0; layerIx < MAX_MATERIAL_TEXTURE_LAYERS; ++layerIx)
	{
		mTextureLayer[layerIx] = NULL;
	}
}

void Material::SetTexture(int layerIx, wchar_t* texFileName)	// TODO:不应该直接从文件中CreateTexture再Set, 增加TextureManager类
{
	if(layerIx < 0 || layerIx >= MAX_MATERIAL_TEXTURE_LAYERS)
		return;

	if(mTextureLayer[layerIx])
		SAFE_DROP(mTextureLayer[layerIx]);

	mTextureLayer[layerIx] = gEngine->GetTextureManger()->GetOrCreateD3DTexture(texFileName);
	mTextureLayer[layerIx]->Grab();	
}

Texture* Material::GetTexture(int layerIx)
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
	switch(shadingMethod)
	{
	case InvalidMethod:
		_Assert(mShader == NULL);
		return;

	case Flat:
		SAFE_DROP(mShader);
		mShader = new FlatShader(this);
		break;

	case View:
		SAFE_DROP(mShader);
		mShader = new ViewShader(this);
		break;

	case Diffuse:
		SAFE_DROP(mShader);
		mShader = new DiffuseShader(this);
		break;

	case Specular:
		SAFE_DROP(mShader);
		mShader = new SpecularShader(this);
		break;

	case BumpSpecular:
		SAFE_DROP(mShader);
		mShader = new BumpSpecularShader(this);
		mShader->SetColorTex(L"./Assets/Textures/white_128x128.jpg");
		mShader->SetNormalTex(L"./Assets/Textures/default_normal_128x128.jpg");
		break;

	default:
		_Assert(false);
		break;
	}

	mShadingMethod = shadingMethod;
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

void Material::Render(const D3DXMATRIX& matWorld, Geometry* geo, Camera* camera, bool isStandAlone /*= false*/)
{
	_Assert(NULL != mShader);
	_Assert(NULL != geo);
	_Assert(NULL != camera);

	mShader->Render(matWorld, geo, camera, isStandAlone);
}

wchar_t* Material::GetName()
{
	return mName;
}

void Material::SetID(DWORD id)
{
	mID = id;
}

ShadingMethod Material::GetShadingMethod()
{
	return mShadingMethod;
}

float Material::GetTilesU()
{
	return mTilesU;
}

float Material::GetTilesV()
{
	return mTilesV;
}

float Material::GetOffsetU()
{
	return mOffsetU;
}

float Material::GetOffsetV()
{
	return mOffsetV;
}

int Material::GetTextureLayerCounts()
{
	int counts = 0;

	for(int layerIx = 0; layerIx < MAX_MATERIAL_TEXTURE_LAYERS; ++layerIx)
	{
		if(mTextureLayer[layerIx])
			counts++;
		else
			break;
	}

	return counts;
}

void Material::SetTilesU(float tilesU)
{
	mTilesU = tilesU;
}

void Material::SetTilesV(float tilesV)
{
	mTilesV = tilesV;
}

void Material::SetOffsetU(float offsetU)
{
	mOffsetU = offsetU;
}

void Material::SetOffsetV(float offsetV)
{
	mOffsetV = offsetV;
}

//bool Material::AcceptGeometry(Geometry* geo)
//{
//	bool accept = true;
//
//	VertexType vertexType = geo->GetVertexType();
//
//	if(vertexType == XYZ_N)
//	{
//		if(mShadingMethod == )
//	}
//}
