#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include "Texture.h"
#include <functional>
#include <hash_map>

struct WCharLess : public std::binary_function<const wchar_t*, const wchar_t*, bool>
{
public:
	result_type operator()(const first_argument_type& left, const second_argument_type& right) const
	{
		return (_wcsicmp(left, right) < 0 ? true : false);
	}
};

typedef stdext::hash_map<const wchar_t*, Texture*, stdext::hash_compare<const wchar_t*, WCharLess>> TexHashMap;

class TextureManager
{
public:
	Texture* GetOrCreateD3DTexture(const wchar_t* filePath);

	void Destory();

	TexHashMap GetTextureHashMap();

private:
	TexHashMap mTextures;
};

#endif