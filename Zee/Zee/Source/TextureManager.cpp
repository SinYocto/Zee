#include "TextureManager.h"
#include "Math.h"
#include "Engine.h"

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

	mTextures.Destory();
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

