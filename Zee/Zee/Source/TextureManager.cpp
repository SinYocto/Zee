#include "TextureManager.h"
#include "Math.h"
#include "Engine.h"

IDirect3DTexture9* TextureManager::GetOrCreateD3DTexture(const wchar_t* filePath)
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
		resultTexture->CreateFromFile(filePath);

		mTextures[filePath] = resultTexture;
	}

	return resultTexture->GetD3DTexture();
}

