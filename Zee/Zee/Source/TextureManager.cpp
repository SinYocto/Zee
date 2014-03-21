#include "TextureManager.h"
#include "Math.h"
#include "Engine.h"

void TextureManager::OnLostDevice()
{
	int size = mCubeTextures.GetSize();
	for(int i = 0; i < size; ++i)
	{
		std::vector<CubeTexture*> texList = mCubeTextures.GetDataList(i);

		for(std::vector<CubeTexture*>::iterator iter = texList.begin(); iter != texList.end(); ++iter)
		{
			(*iter)->OnLostDevice();
		}
	}
}

void TextureManager::OnResetDevice()
{
	int size = mCubeTextures.GetSize();
	for(int i = 0; i < size; ++i)
	{
		std::vector<CubeTexture*> texList = mCubeTextures.GetDataList(i);

		for(std::vector<CubeTexture*>::iterator iter = texList.begin(); iter != texList.end(); ++iter)
		{
			(*iter)->OnResetDevice();
		}
	}
}

Texture* TextureManager::GetOrCreateTexture(const wchar_t* filePath)
{
	Texture* resultTexture = NULL;

	resultTexture = mTextures.Find(filePath);
	if(resultTexture == NULL)
	{
		_Assert(YFile::Exist(filePath));

		resultTexture = New Texture();
		resultTexture->CreateFromFile(filePath);

		mTextures.Insert(filePath, resultTexture);
	}

	return resultTexture;
}

CubeTexture* TextureManager::GetCubeTexture(const wchar_t* name)
{
	CubeTexture* resultCubeTexture = NULL;

	resultCubeTexture = mCubeTextures.Find(name);
	_Assert(resultCubeTexture != NULL);

	return resultCubeTexture;
}

CubeTexture* TextureManager::CreateCubeTexture(const wchar_t* name, const wchar_t* filePathPX, const wchar_t* filePathNX, 
											   const wchar_t* filePathPY, const wchar_t* filePathNY, 
											   const wchar_t* filePathPZ, const wchar_t* filePathNZ)
{
	CubeTexture* resultCubeTexture = New CubeTexture(name);
	resultCubeTexture->CreateFromFile(filePathPX, filePathNX, filePathPY, filePathNY, filePathPZ, filePathNZ);
	_Assert(resultCubeTexture != NULL);

	mCubeTextures.Insert(name, resultCubeTexture);

	return resultCubeTexture;
}

void TextureManager::Destory()
{
	int size = mTextures.GetSize();
	for(int i = 0; i < size; ++i)
	{
		std::vector<Texture*> texList = mTextures.GetDataList(i);

		for(std::vector<Texture*>::iterator iter = texList.begin(); iter != texList.end(); ++iter)
		{
			SAFE_DROP(*iter);
		}
	}

	size = mCubeTextures.GetSize();
	for(int i = 0; i < size; ++i)
	{
		std::vector<CubeTexture*> texList = mCubeTextures.GetDataList(i);

		for(std::vector<CubeTexture*>::iterator iter = texList.begin(); iter != texList.end(); ++iter)
		{
			SAFE_DROP(*iter);
		}
	}

	mTextures.Destory();
	mCubeTextures.Destory();
}

std::vector<Texture*> TextureManager::GetAllTextures()
{
	std::vector<Texture*> totalTexList;

	int size = mTextures.GetSize();
	for(int i = 0; i < size; ++ i)
	{
		std::vector<Texture*> texList = mTextures.GetDataList(i);
		for(std::vector<Texture*>::iterator iter = texList.begin(); iter != texList.end(); ++iter)
		{
			if(*iter != NULL)
				totalTexList.push_back(*iter);
		}
	}

	return totalTexList;
}

