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
	Material(const wchar_t* _name)
		:shader(NULL)
		,diffuseColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f))
		,specularColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f))
		,shiness(1.0f)
		,gloss(50.0f)
		,tilesU(1.0f)
		,tilesV(1.0f)
		,offsetU(0)
		,offsetV(0)
	{
		YString::Copy(name, _countof(name), _name);

		for(int layerIx = 0; layerIx < MAX_MATERIAL_TEXTURE_LAYERS; ++layerIx)
		{
			textureLayer[layerIx] = NULL;
		}
	}

	~Material()
	{
		for(int layerIx = 0; layerIx < MAX_MATERIAL_TEXTURE_LAYERS; ++layerIx)
		{
			SAFE_RELEASE(textureLayer[layerIx]);
		}

		SAFE_DELETE(shader);
	}

	void SetShader(ShadingMethod shadingMethod);

	void SetTexture(int layerIx, wchar_t* texFileName);	// TODO: 改为使用texture,而不是直接从文件创建texture
	IDirect3DTexture9* GetTexture(int layerIx);

	void SetDiffuseColor(D3DXCOLOR color);
	D3DXCOLOR GetDiffuseColor();

	void SetSpecularColor(D3DXCOLOR color);
	D3DXCOLOR GetSpecularColor();
	D3DXCOLOR GetFinalSpecularColor();

	void SetSpecShiness(float _shiness);
	float GetSpecShiness();

	void SetSpecGloss(float _gloss);
	float GetSpecGloss();

	void SetUVTiles(float _tilesU, float _tilesV);
	void SetUVOffset(float _offsetU, float _offsetV);

	D3DXMATRIX UVTransformMatrix();

	void Render(Object* object, Mesh* mesh, Camera* camera);

	wchar_t* GetName();
	void SetID(DWORD _id);

public:
	IShader* shader;

private:
	DWORD id;
	wchar_t name[MAX_STR_LEN];

	D3DXCOLOR diffuseColor;
	D3DXCOLOR specularColor;
	float gloss;
	float shiness;
	
	float tilesU;
	float tilesV;

	float offsetU;
	float offsetV;

	Vector4 paramsVec1;
	Vector4 paramsVec2;
	
	IDirect3DTexture9* textureLayer[MAX_MATERIAL_TEXTURE_LAYERS];
};

#endif