#ifndef MATERIAL_H
#define MATERIAL_H

#include "D3DUtility.h"
#include "IReferenceCounted.h"
#include "Shader.h"
#include "Texture.h"

#define MAX_MATERIAL_TEXTURE_LAYERS 4

class Object;
class Texture;
class Geometry;

struct MaterialData
{
	MaterialData()
		:shadingMethod(InvalidMethod)
		,ambientColor(D3DXCOLOR_WHITE)
		,diffColor(D3DXCOLOR_WHITE)
		,specColor(D3DXCOLOR_WHITE)
		,shiness(1.0f)
		,gloss(50.0f)
		,tilesU(1.0f)
		,tilesV(1.0f)
		,offsetU(0)
		,offsetV(0)
	{
		for(int i = 0; i < MAX_MATERIAL_TEXTURE_LAYERS; ++i)
		{
			YString::Empty(texFilesPath[i]);
		}
	}

	ShadingMethod shadingMethod;

	D3DXCOLOR ambientColor;
	D3DXCOLOR diffColor;
	D3DXCOLOR specColor;

	float gloss;
	float shiness;

	float tilesU;
	float tilesV;

	float offsetU;
	float offsetV;

	wchar_t texFilesPath[MAX_MATERIAL_TEXTURE_LAYERS][MAX_PATH_LEN];
};

class Material : public IReferenceCounted
{
public:
	Material(const wchar_t* name, const wchar_t* filePath = NULL);
	Material(const Material& mtl);		// 用于复制一个临时使用的mtl, 不纳入manager管理, 不关心id和name

	~Material();

	wchar_t* GetName();
	void SetID(DWORD id);

	void SetShader(ShadingMethod shadingMethod);
	void SetTexture(int layerIx, wchar_t* texFileName);	// TODO: 改为使用texture,而不是直接从文件创建texture

	void SetAmbientColor(D3DXCOLOR color);
	void SetDiffuseColor(D3DXCOLOR color);
	void SetSpecularColor(D3DXCOLOR color);
	void SetSpecShiness(float shiness);
	void SetSpecGloss(float gloss);

	ShadingMethod GetShadingMethod();

	Texture* GetTexture(int layerIx);
	int GetTextureLayerCounts();

	D3DXCOLOR GetAmbientColor();
	D3DXCOLOR GetDiffuseColor();
	D3DXCOLOR GetSpecularColor();
	D3DXCOLOR GetFinalSpecularColor();
	float GetSpecShiness();
	float GetSpecGloss();

	float GetTilesU();
	float GetTilesV();
	float GetOffsetU();
	float GetOffsetV();

	void SetTilesU(float tilesU);
	void SetTilesV(float tilesV);
	void SetOffsetU(float offsetU);
	void SetOffsetV(float offsetV);

	void SetUVTiles(float _tilesU, float _tilesV);
	void SetUVOffset(float _offsetU, float _offsetV);

	//bool AcceptGeometry(Geometry* geo);

	D3DXMATRIX UVTransformMatrix();

	void Render(const D3DXMATRIX& matWorld, Geometry* geo, Camera* camera, bool isStandAlone = false);

	void SaveToFile(const wchar_t* dirPath);
	void LoadDataFromFile(const wchar_t* filePath);

	wchar_t* GetFilePath();

public:
	IShader* mShader;

private:
	DWORD mID;
	wchar_t mName[MAX_STR_LEN];

	wchar_t mFilePath[MAX_PATH_LEN];

	MaterialData mMtlData;

	Vector4 mParamsVec1;
	Vector4 mParamsVec2;
	
	Texture* mTextureLayer[MAX_MATERIAL_TEXTURE_LAYERS];
};

#endif