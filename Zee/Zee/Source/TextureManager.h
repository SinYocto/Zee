#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include "Texture.h"
#include "CubeTexture.h"
#include "HashMap.h"

typedef HashMap<Texture> TexHashMap;
typedef HashMap<CubeTexture> CubeTexHashMap;

class TextureManager
{
public:
	void OnLostDevice();		// CubeTextureÐèÒª
	void OnResetDevice();

	Texture* GetOrCreateTexture(const wchar_t* filePath);
	CubeTexture* GetCubeTexture(const wchar_t* name);
	CubeTexture* CreateCubeTexture(const wchar_t* name, const wchar_t* filePathPX, const wchar_t* filePathNX, 
		const wchar_t* filePathPY, const wchar_t* filePathNY, 
		const wchar_t* filePathPZ, const wchar_t* filePathNZ);

	void Destory();

	std::vector<Texture*> GetAllTextures();

private:
	TexHashMap mTextures;
	CubeTexHashMap mCubeTextures;
};

#endif