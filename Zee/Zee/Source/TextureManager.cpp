#include "TextureManager.h"
#include "Math.h"
#include "Engine.h"

Texture* TextureManager::GetOrCreateD3DTexture(const wchar_t* filePath)
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
		resultTexture = new Texture();
		resultTexture->CreateFromFile(filePath);	// TODO:文件不存在, 创建失败时

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

