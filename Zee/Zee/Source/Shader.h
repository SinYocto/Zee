#ifndef SHADER_H
#define SHADER_H

#include"D3DUtility.h"

enum ShadingMethod { Diffuse, Specular, BumpSpecular };

class Material;
class Camera;
class Object;
class Mesh;

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

	virtual void SetSpecShiness(float _shiness)
	{

	}
	virtual void SetSpecGloss(float _gloss)
	{

	}

	virtual void SetUVTiles(float _tilesU, float _tilesV)
	{

	}
	virtual void SetUVOffset(float _offsetU, float _offsetV)
	{

	}

	virtual void Render(Object* object, Mesh* mesh, Camera* camera)
	{

	}
};

class DiffuseShader : public IShader
{
public:
	DiffuseShader(Material* _material)
		:material(_material)
	{

	}

	static void OnLostDevice()
	{
		effect->OnLostDevice();
	}

	static void OnResetDevice()
	{
		effect->OnResetDevice();
	}

	static void CreateEffectFromFile(wchar_t* FXFileName)
	{
		D3DXCreateEffectFromFile(gD3DDevice, FXFileName, NULL, NULL, D3DXSHADER_DEBUG, NULL, &effect, NULL);
	}

	void SetColorTex(wchar_t* texFileName);

	void SetAmbientColor(D3DXCOLOR color); 
	void SetDiffuseColor(D3DXCOLOR color);

	void SetUVTiles(float _tilesU, float _tilesV);
	void SetUVOffset(float _offsetU, float _offsetV);

	void Render(Object* object, Mesh* mesh, Camera* camera);

private:
	Material* material;

public:
	static LPD3DXEFFECT effect;
};

class SpecularShader : public IShader
{
public:
	SpecularShader(Material* _material)
		:material(_material)
	{

	}

	static void OnLostDevice()
	{
		effect->OnLostDevice();
	}

	static void OnResetDevice()
	{
		effect->OnResetDevice();
	}

	static void CreateEffectFromFile(wchar_t* FXFileName)
	{
		D3DXCreateEffectFromFile(gD3DDevice, FXFileName, NULL, NULL, D3DXSHADER_DEBUG, NULL, &effect, NULL);
	}

	void SetColorTex(wchar_t* texFileName);

	void SetAmbientColor(D3DXCOLOR color);
	void SetDiffuseColor(D3DXCOLOR color);
	void SetSpecularColor(D3DXCOLOR color);

	void SetSpecShiness(float _shiness);
	void SetSpecGloss(float _gloss);

	void SetUVTiles(float _tilesU, float _tilesV);
	void SetUVOffset(float _offsetU, float _offsetV);

	void Render(Object* object, Mesh* mesh, Camera* camera);

private:
	Material* material;

public:
	static LPD3DXEFFECT effect;
};

class BumpSpecularShader : public IShader
{
public:
	BumpSpecularShader(Material* _material)
		:material(_material)
	{

	}

	static void OnLostDevice()
	{
		effect->OnLostDevice();
	}

	static void OnResetDevice()
	{
		effect->OnResetDevice();
	}

	static void CreateEffectFromFile(wchar_t* FXFileName)
	{
		D3DXCreateEffectFromFile(gD3DDevice, FXFileName, NULL, NULL, D3DXSHADER_DEBUG, NULL, &effect, NULL);
	}

	void SetColorTex(wchar_t* texFileName);
	void SetNormalTex(wchar_t* texFileName);

	void SetAmbientColor(D3DXCOLOR color);
	void SetDiffuseColor(D3DXCOLOR color);
	void SetSpecularColor(D3DXCOLOR color);

	void SetSpecShiness(float _shiness);
	void SetSpecGloss(float _gloss);

	void SetUVTiles(float _tilesU, float _tilesV);
	void SetUVOffset(float _offsetU, float _offsetV);

	void Render(Object* object, Mesh* mesh, Camera* camera);

private:
	Material* material;

public:
	static LPD3DXEFFECT effect;
};

#endif