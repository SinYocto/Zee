#include"Material.h"

void Material::SetTexture(int layerIx, wchar_t* texFileName)	// TODO:不应该直接从文件中CreateTexture再Set, 增加TextureManager类
{
	if(layerIx < 0 || layerIx >= MAX_MATERIAL_TEXTURE_LAYERS)
		return;

	if(textureLayer[layerIx])
		SAFE_RELEASE(textureLayer[layerIx]);

	D3DXCreateTextureFromFile(gD3DDevice, texFileName, &textureLayer[layerIx]);		
}

IDirect3DTexture9* Material::GetTexture(int layerIx)
{
	if(layerIx < 0 || layerIx >= MAX_MATERIAL_TEXTURE_LAYERS)
		return NULL;

	return textureLayer[layerIx];
}

D3DXMATRIX Material::UVTransformMatrix()
{
	D3DXMATRIX matScale, matTrans;
	D3DXMatrixScaling(&matScale, tilesU, tilesV, 1.0f);
	D3DXMatrixTranslation(&matTrans, offsetU, offsetV, 0);

	return matScale * matTrans;
}

void Material::SetShader(ShadingMethod shadingMethod)
{
	SAFE_DELETE(shader);

	switch(shadingMethod)
	{
	case Diffuse:
		shader = new DiffuseShader(this);
		break;
	case Specular:
		shader = new SpecularShader(this);
		break;;
	case BumpSpecular:
		shader = new BumpSpecularShader(this);
		break;
	}

	_Assert(NULL != shader);
}

void Material::SetAmbientColor(D3DXCOLOR color)
{
	ambientColor = color;
}

D3DXCOLOR Material::GetAmbientColor()
{
	return ambientColor;
}

void Material::SetDiffuseColor(D3DXCOLOR color)
{
	diffuseColor = color;
}

D3DXCOLOR Material::GetDiffuseColor()
{
	return diffuseColor;
}

void Material::SetSpecularColor(D3DXCOLOR color)
{
	specularColor = color;
}

D3DXCOLOR Material::GetSpecularColor()
{
	return specularColor;
}

D3DXCOLOR Material::GetFinalSpecularColor()
{
	return shiness * specularColor;
}

void Material::SetSpecShiness(float _shiness)
{
	shiness = _shiness;
}

float Material::GetSpecShiness()
{
	return shiness;
}

void Material::SetSpecGloss(float _gloss)
{
	gloss = _gloss;
}

float Material::GetSpecGloss()
{
	return gloss;
}

void Material::SetUVTiles(float _tilesU, float _tilesV)
{
	tilesU = _tilesU;
	tilesV = _tilesV;
}

void Material::SetUVOffset(float _offsetU, float _offsetV)
{
	offsetU = _offsetU;
	offsetV = _offsetV;
}

void Material::Render(Object* object, Geometry* geo, Camera* camera)
{
	_Assert(NULL != shader);
	_Assert(NULL != geo);
	_Assert(NULL != camera);

	shader->Render(object, geo, camera);
}

wchar_t* Material::GetName()
{
	return name;
}

void Material::SetID(DWORD _id)
{
	id = _id;
}
