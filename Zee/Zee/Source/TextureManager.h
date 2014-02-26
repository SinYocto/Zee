#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include "Texture.h"
#include "HashMap.h"

typedef HashMap<Texture> TexHashMap;

class TextureManager
{
public:
	Texture* GetOrCreateTexture(const wchar_t* filePath);

	void Destory();

	std::vector<Texture*> GetAllTextures();

private:
	TexHashMap mTextures;
};

#endif