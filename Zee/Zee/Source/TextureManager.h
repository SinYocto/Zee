#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include "Texture.h"
#include "ResourceHashMap.h"

typedef stdext::hash_map<const wchar_t*, Texture*, stdext::hash_compare<const wchar_t*, WCharLess>> TexHashMap;

class TextureManager
{
public:
	Texture* GetOrCreateTexture(const wchar_t* filePath);

	void Destory();

	TexHashMap GetTextureHashMap();

private:
	TexHashMap mTextures;
};

#endif