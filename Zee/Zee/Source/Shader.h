#ifndef SHADER_H
#define SHADER_H

#include"D3DUtility.h"

enum ShadingMethod { Diffuse, Specular, BumpSpecular };

class Material;
class Camera;
class Object;
class Geometry;

interface IShader
{
public:
	virtual void SetColorTex(wchar_t* texFileName)
	{

	}
	virtual void SetNormalTex(wchar_t* texFileName)
	{

	}

	virtual void SetAmbientColor(D3DXCOLOR color)
	{

	}

	virtual void SetDiffuseColor(D3DXCOLOR color)
	{

	}
	virtual void SetSpecularColor(D3DXCOLOR color)
	{

	}

	virtual void SetSpecShiness(float shiness)
	{

	}
	virtual void SetSpecGloss(float gloss)
	{

	}

	virtual void SetUVTiles(float tilesU, float tilesV)
	{

	}
	virtual void SetUVOffset(float offsetU, float offsetV)
	{

	}

	virtual void Render(Object* object, Geometry* geo, Camera* camera)
	{

	}
};

class DiffuseShader : public IShader
{
public:
	DiffuseShader(Material* material)
		:mMaterial(material)
	{

	}

	static void OnLostDevice()
	{
		mEffect->OnLostDevice();
	}

	static void OnResetDevice()
	{
		mEffect->OnResetDevice();
	}

	static void CreateEffectFromFile(wchar_t* FXFileName)
	{
		D3DXCreateEffectFromFile(gD3DDevice, FXFileName, NULL, NULL, D3DXSHADER_DEBUG, NULL, &mEffect, NULL);
	}

	void SetColorTex(wchar_t* texFileName);

	void SetAmbientColor(D3DXCOLOR color); 
	void SetDiffuseColor(D3DXCOLOR color);

	void SetUVTiles(float tilesU, float tilesV);
	void SetUVOffset(float offsetU, float offsetV);

	void Render(Object* object, Geometry* geo, Camera* camera);

private:
	Material* mMaterial;

public:
	static LPD3DXEFFECT mEffect;
};

class SpecularShader : public IShader
{
public:
	SpecularShader(Material* _material)
		:mMaterial(_material)
	{

	}

	static void OnLostDevice()
	{
		mEffect->OnLostDevice();
	}

	static void OnResetDevice()
	{
		mEffect->OnResetDevice();
	}

	static void CreateEffectFromFile(wchar_t* FXFileName)
	{
		D3DXCreateEffectFromFile(gD3DDevice, FXFileName, NULL, NULL, D3DXSHADER_DEBUG, NULL, &mEffect, NULL);
	}

	void SetColorTex(wchar_t* texFileName);

	void SetAmbientColor(D3DXCOLOR color);
	void SetDiffuseColor(D3DXCOLOR color);
	void SetSpecularColor(D3DXCOLOR color);

	void SetSpecShiness(float shiness);
	void SetSpecGloss(float gloss);

	void SetUVTiles(float tilesU, float tilesV);
	void SetUVOffset(float offsetU, float offsetV);

	void Render(Object* object, Geometry* geo, Camera* camera);

private:
	Material* mMaterial;

public:
	static LPD3DXEFFECT mEffect;
};

class BumpSpecularShader : public IShader
{
public:
	BumpSpecularShader(Material* material)
		:mMaterial(material)
	{

	}

	static void OnLostDevice()
	{
		mEffect->OnLostDevice();
	}

	static void OnResetDevice()
	{
		mEffect->OnResetDevice();
	}

	static void CreateEffectFromFile(wchar_t* FXFileName)
	{
		D3DXCreateEffectFromFile(gD3DDevice, FXFileName, NULL, NULL, D3DXSHADER_DEBUG, NULL, &mEffect, NULL);
	}

	void SetColorTex(wchar_t* texFileName);
	void SetNormalTex(wchar_t* texFileName);

	void SetAmbientColor(D3DXCOLOR color);
	void SetDiffuseColor(D3DXCOLOR color);
	void SetSpecularColor(D3DXCOLOR color);

	void SetSpecShiness(float shiness);
	void SetSpecGloss(float gloss);

	void SetUVTiles(float tilesU, float tilesV);
	void SetUVOffset(float offsetU, float offsetV);

	void Render(Object* object, Geometry* geo, Camera* camera);

private:
	Material* mMaterial;

public:
	static LPD3DXEFFECT mEffect;
};

#endif