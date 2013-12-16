#include "TextureManager.h"
#include "Math.h"
#include "Engine.h"

Texture* TextureManager::GetOrCreateTexture(const wchar_t* filePath)
{
	Texture* resultTexture = NULL;

	TexHashMap::iterator iter = mTextures.find(filePath);
	if(iter != mTextures.end())
	{
		resultTexture = iter->second;
		_Assert(NULL != resultTexture);
	}
	else
	{
		_Assert(YFile::Exist(filePath));

		resultTexture = New Texture();
		resultTexture->CreateFromFile(filePath);

		mTextures[filePath] = resultTexture;
	}

	return resultTexture;
}

void TextureManager::Destory()
{
	for(TexHashMap::iterator iter = mTextures.begin(); iter != mTextures.end(); ++iter)
	{
		SAFE_DROP(iter->second);
	}

	mTextures.clear();
}

TexHashMap TextureManager::GetTextureHashMap()
{
	return mTextures;
}

