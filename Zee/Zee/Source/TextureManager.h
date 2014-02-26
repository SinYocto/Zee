#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include "Texture.h"
//#include "ResourceHashMap.h"
#include "HashMap.h"

//// TODO: ʹ��hash_map��������, ���������ԭ��δ֪
//typedef stdext::hash_map<const wchar_t*, Texture*, stdext::hash_compare<const wchar_t*, WCharLess>> TexHashMap;

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