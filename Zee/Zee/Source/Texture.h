#ifndef TEXTURE_H
#define TEXTURE_H

#include "D3DUtility.h"
#include "IReferenceCounted.h"

class Texture : public IReferenceCounted
{
public:
	Texture();
	~Texture();

	void CreateFromFile(const wchar_t* filePath);

	wchar_t* GetFilePath();
	IDirect3DTexture9* GetD3DTexture();

private:
	wchar_t mFilePath[MAX_STR_LEN];
	wchar_t mName[MAX_STR_LEN];

	IDirect3DTexture9* mD3DTexture;
};


#endif