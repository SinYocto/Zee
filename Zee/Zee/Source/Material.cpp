#include "Material.h"
#include "Engine.h"
#include "IDAllocator.h"

Material::Material(const wchar_t* name, const wchar_t* filePath /*= NULL*/) 
:mID(IDAllocator::ID_ANY)
,mShader(NULL)
,mMtlData()
{
	YString::Empty(mFilePath);
	YString::Copy(mName, _countof(mName), name);

	for(int layerIx = 0; layerIx < MAX_MATERIAL_TEXTURE_LAYERS; ++layerIx)
	{
		mTextureLayer[layerIx] = NULL;
	}

	if(NULL != filePath)
	{
		LoadDataFromFile(filePath);
		YString::Copy(mFilePath, _countof(mFilePath), filePath);
	}
}

Material::Material( const Material& mtl ) :mParamsVec1(mtl.mParamsVec1)
,mParamsVec2(mtl.mParamsVec2)
,mShader(NULL)
{
	mMtlData.ambientColor = mtl.mMtlData.ambientColor;
	mMtlData.diffColor = mtl.mMtlData.diffColor;
	mMtlData.specColor = mtl.mMtlData.specColor;
	mMtlData.shiness = mtl.mMtlData.shiness;
	mMtlData.gloss = mtl.mMtlData.gloss;
	mMtlData.tilesU = mtl.mMtlData.tilesU;
	mMtlData.tilesV = mtl.mMtlData.tilesV;
	mMtlData.offsetU = mtl.mMtlData.offsetU;
	mMtlData.offsetV = mtl.mMtlData.offsetV;
	mMtlData.shadingMethod = mtl.mMtlData.shadingMethod;

	for(int i = 0; i < MAX_MATERIAL_TEXTURE_LAYERS; ++i)
	{
		YString::Copy(mMtlData.texFilesPath[i], MAX_PATH_LEN, mtl.mMtlData.texFilesPath[i]);
	}

	YString::Copy(mName, _countof(mName), L"clone");

	SetShader(mMtlData.shadingMethod);

	for(int layerIx = 0; layerIx < MAX_MATERIAL_TEXTURE_LAYERS; ++layerIx)
	{
		mTextureLayer[layerIx] = mtl.mTextureLayer[layerIx];

		if(mTextureLayer[layerIx])
			mTextureLayer[layerIx]->Grab();
	}
}

Material::~Material()
{
	for(int layerIx = 0; layerIx < MAX_MATERIAL_TEXTURE_LAYERS; ++layerIx)
	{
		SAFE_DROP(mTextureLayer[layerIx]);
	}

	SAFE_DROP(mShader);
}


void Material::SetTexture(int layerIx, wchar_t* texFileName)	// TODO:不应该直接从文件中CreateTexture再Set, 增加TextureManager类
{
	if(layerIx < 0 || layerIx >= MAX_MATERIAL_TEXTURE_LAYERS)
		return;

	if(mTextureLayer[layerIx])
		SAFE_DROP(mTextureLayer[layerIx]);

	mTextureLayer[layerIx] = gEngine->GetTextureManger()->GetOrCreateTexture(texFileName);
	mTextureLayer[layerIx]->Grab();	

	YString::Copy(mMtlData.texFilesPath[layerIx], MAX_PATH_LEN, mTextureLayer[layerIx]->GetFilePath());
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
	D3DXMatrixScaling(&matScale, mMtlData.tilesU, mMtlData.tilesV, 1.0f);
	D3DXMatrixTranslation(&matTrans, mMtlData.offsetU, mMtlData.offsetV, 0);

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
		mShader = New FlatShader(this);
		break;

	case View:
		SAFE_DROP(mShader);
		mShader = New ViewShader(this);
		break;

	case Diffuse:
		SAFE_DROP(mShader);
		mShader = New DiffuseShader(this);
		break;

	case Specular:
		SAFE_DROP(mShader);
		mShader = New SpecularShader(this);
		break;

	case BumpSpecular:
		SAFE_DROP(mShader);
		mShader = New BumpSpecularShader(this);
		mShader->SetColorTex(L"./Assets/Textures/white_128x128.jpg");
		mShader->SetNormalTex(L"./Assets/Textures/default_normal_128x128.jpg");
		break;

	default:
		_Assert(false);
		break;
	}

	mMtlData.shadingMethod = shadingMethod;
	_Assert(NULL != mShader);
}

void Material::SetAmbientColor(D3DXCOLOR color)
{
	mMtlData.ambientColor = color;
}

D3DXCOLOR Material::GetAmbientColor()
{
	return mMtlData.ambientColor;
}

void Material::SetDiffuseColor(D3DXCOLOR color)
{
	mMtlData.diffColor = color;
}

D3DXCOLOR Material::GetDiffuseColor()
{
	return mMtlData.diffColor;
}

void Material::SetSpecularColor(D3DXCOLOR color)
{
	mMtlData.specColor = color;
}

D3DXCOLOR Material::GetSpecularColor()
{
	return mMtlData.specColor;
}

D3DXCOLOR Material::GetFinalSpecularColor()
{
	return mMtlData.shiness * mMtlData.specColor;
}

void Material::SetSpecShiness(float shiness)
{
	mMtlData.shiness = shiness;
}

float Material::GetSpecShiness()
{
	return mMtlData.shiness;
}

void Material::SetSpecGloss(float gloss)
{
	mMtlData.gloss = gloss;
}

float Material::GetSpecGloss()
{
	return mMtlData.gloss;
}

void Material::SetUVTiles(float tilesU, float tilesV)
{
	mMtlData.tilesU = tilesU;
	mMtlData.tilesV = tilesV;
}

void Material::SetUVOffset(float offsetU, float offsetV)
{
	mMtlData.offsetU = offsetU;
	mMtlData.offsetV = offsetV;
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
	return mMtlData.shadingMethod;
}

float Material::GetTilesU()
{
	return mMtlData.tilesU;
}

float Material::GetTilesV()
{
	return mMtlData.tilesV;
}

float Material::GetOffsetU()
{
	return mMtlData.offsetU;
}

float Material::GetOffsetV()
{
	return mMtlData.offsetV;
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
	mMtlData.tilesU = tilesU;
}

void Material::SetTilesV(float tilesV)
{
	mMtlData.tilesV = tilesV;
}

void Material::SetOffsetU(float offsetU)
{
	mMtlData.offsetU = offsetU;
}

void Material::SetOffsetV(float offsetV)
{
	mMtlData.offsetV = offsetV;
}

void Material::SaveToFile(const wchar_t* dirPath)
{
	wchar_t filePath[MAX_PATH_LEN];
	YString::Copy(filePath, _countof(filePath), dirPath);
	YString::Concat(filePath, _countof(filePath), L"/");
	YString::Concat(filePath, _countof(filePath), mName, L".material");

	if(YString::isEmpty(mFilePath))
	{
		YString::Copy(mFilePath, _countof(mFilePath), filePath);
	}

	if(YFile::Exist(filePath))
	{
		Log(L"warning: file already exists. Material::SaveToFile(%s)\n", filePath);
		return;
	}

	// TODO: dirPath不存在

	YFile* file = YFile::Open(filePath, YFile::WRITE);

	file->WriteLine(L"ShadingMethod %d", mMtlData.shadingMethod);
	file->WriteLine(L"ambientColor %.2f %.2f %.2f", mMtlData.ambientColor.r, mMtlData.ambientColor.g, mMtlData.ambientColor.b);
	file->WriteLine(L"diffColor %.2f %.2f %.2f", mMtlData.diffColor.r, mMtlData.diffColor.g, mMtlData.diffColor.b);
	file->WriteLine(L"specColor %.2f %.2f %.2f", mMtlData.specColor.r, mMtlData.specColor.g, mMtlData.specColor.b);
	file->WriteLine(L"shiness %.2f", mMtlData.shiness);
	file->WriteLine(L"gloss %.2f", mMtlData.gloss);
	file->WriteLine(L"uvTiles %.2f %.2f", mMtlData.tilesU, mMtlData.tilesV);
	file->WriteLine(L"uvOffset %.2f %.2f", mMtlData.offsetU, mMtlData.offsetV);

	for(int i = 0; i < MAX_MATERIAL_TEXTURE_LAYERS; ++i)
	{
		wchar_t path[MAX_PATH_LEN];
		YString::Copy(path, _countof(path), L"null");

		if(!YString::isEmpty(mMtlData.texFilesPath[i]))
			YString::Copy(path, _countof(path), mMtlData.texFilesPath[i]);

		file->WriteLine(L"tex%d %s", i, path);
	}

	file->Close();
}

void Material::LoadDataFromFile(const wchar_t* filePath)
{	
	_Assert(YFile::Exist(filePath));

	wchar_t suffix[MAX_STR_LEN];
	YString::GetFileSuffix(suffix, _countof(suffix), filePath);
	_Assert(YString::Compare(suffix, L"material", true) == 0);

	YFile* file = YFile::Open(filePath, YFile::READ);

	wchar_t lineContent[MAX_STR_LEN];
	while(file->ReadLine(lineContent, MAX_STR_LEN) != NULL)
	{
		wchar_t specifier[MAX_STR_LEN];
		YString::GetSpecifier(specifier, _countof(specifier), lineContent);

		if(YString::Compare(specifier, L"ShadingMethod", true) == 0)
		{
			YString::Scan(lineContent, L"%*s %d", &mMtlData.shadingMethod);
			SetShader(mMtlData.shadingMethod);
		}
		else if(YString::Compare(specifier, L"ambientColor", true) == 0)
		{
			YString::Scan(lineContent, L"%*s %f %f %f", 
				&mMtlData.ambientColor.r, &mMtlData.ambientColor.g, &mMtlData.ambientColor.b);
		}
		else if(YString::Compare(specifier, L"diffColor", true) == 0)
		{
			YString::Scan(lineContent, L"%*s %f %f %f", 
				&mMtlData.diffColor.r, &mMtlData.diffColor.g, &mMtlData.diffColor.b);
		}
		else if(YString::Compare(specifier, L"specColor", true) == 0)
		{
			YString::Scan(lineContent, L"%*s %f %f %f", 
				&mMtlData.specColor.r, &mMtlData.specColor.g, &mMtlData.specColor.b);
		}
		else if(YString::Compare(specifier, L"shiness", true) == 0)
		{
			YString::Scan(lineContent, L"%*s %f", &mMtlData.shiness);
		}
		else if(YString::Compare(specifier, L"gloss", true) == 0)
		{
			YString::Scan(lineContent, L"%*s %f", &mMtlData.gloss);
		}
		else if(YString::Compare(specifier, L"uvTiles", true) == 0)
		{
			YString::Scan(lineContent, L"%*s %f %f", &mMtlData.tilesU, &mMtlData.tilesV);
		}
		else if(YString::Compare(specifier, L"uvOffset", true) == 0)
		{
			YString::Scan(lineContent, L"%*s %f %f", &mMtlData.offsetU, &mMtlData.offsetV);
		}
		else if(YString::Compare(specifier, L"tex0", true) == 0)
		{
			wchar_t path[MAX_PATH_LEN];
			YString::Scan(lineContent, L"%*s %s", &path);

			if(YString::Compare(path, L"null", true) != 0)
			{
				SetTexture(0, path);
			}
		}
		else if(YString::Compare(specifier, L"tex1", true) == 0)
		{
			wchar_t path[MAX_PATH_LEN];
			YString::Scan(lineContent, L"%*s %s", &path);

			if(YString::Compare(path, L"null", true) != 0)
			{
				SetTexture(1, path);
			}
		}
		else if(YString::Compare(specifier, L"tex2", true) == 0)
		{
			wchar_t path[MAX_PATH_LEN];
			YString::Scan(lineContent, L"%*s %s", &path);

			if(YString::Compare(path, L"null", true) != 0)
			{
				SetTexture(2, path);
			}
		}
		else if(YString::Compare(specifier, L"tex3", true) == 0)
		{
			wchar_t path[MAX_PATH_LEN];
			YString::Scan(lineContent, L"%*s %s", &path);

			if(YString::Compare(path, L"null", true) != 0)
			{
				SetTexture(3, path);
			}
		}
	}

	file->Close();
}

wchar_t* Material::GetFilePath()
{	
	return mFilePath;
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
