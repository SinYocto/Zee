#ifndef MATERIAL_H
#define MATERIAL_H

#include "Utility.h"
#include"D3DUtility.h"
#include "IReferenceCounted.h"
#include "Shader.h"
#include "YString.h"

#define MAX_MATERIAL_TEXTURE_LAYERS 4

class Object;

class Material : public IReferenceCounted
{
public:
	Material(const wchar_t* name)
		:mShader(NULL)
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

	~Material()
	{
		for(int layerIx = 0; layerIx < MAX_MATERIAL_TEXTURE_LAYERS; ++layerIx)
		{
			SAFE_RELEASE(mTextureLayer[layerIx]);
		}

		SAFE_DELETE(mShader);
	}

	wchar_t* GetName();
	void SetID(DWORD id);

	void SetShader(ShadingMethod shadingMethod);
	void SetTexture(int layerIx, wchar_t* texFileName);	// TODO: 改为使用texture,而不是直接从文件创建texture

	void SetAmbientColor(D3DXCOLOR color);
	void SetDiffuseColor(D3DXCOLOR color);
	void SetSpecularColor(D3DXCOLOR color);
	void SetSpecShiness(float shiness);
	void SetSpecGloss(float gloss);

	IDirect3DTexture9* GetTexture(int layerIx);

	D3DXCOLOR GetAmbientColor();
	D3DXCOLOR GetDiffuseColor();
	D3DXCOLOR GetSpecularColor();
	D3DXCOLOR GetFinalSpecularColor();
	float GetSpecShiness();
	float GetSpecGloss();

	void SetUVTiles(float _tilesU, float _tilesV);
	void SetUVOffset(float _offsetU, float _offsetV);

	D3DXMATRIX UVTransformMatrix();

	void Render(Object* object, Geometry* geo, Camera* camera);

public:
	IShader* mShader;

private:
	DWORD mID;
	wchar_t mName[MAX_STR_LEN];

	D3DXCOLOR mAmbientColor;
	D3DXCOLOR mDiffuseColor;
	D3DXCOLOR mSpecularColor;
	float mGloss;
	float mShiness;
	
	float mTilesU;
	float mTilesV;

	float mOffsetU;
	float mOffsetV;

	Vector4 mParamsVec1;
	Vector4 mParamsVec2;
	
	IDirect3DTexture9* mTextureLayer[MAX_MATERIAL_TEXTURE_LAYERS];
};

#endif