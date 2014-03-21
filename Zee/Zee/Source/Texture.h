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
	void Create(int width, int height, D3DFORMAT format, DWORD usage);	// 作为renderTarget创建时需要进行设备恢复

	wchar_t* GetFilePath();
	wchar_t* GetName();
	IDirect3DTexture9* GetD3DTexture();

private:
	wchar_t mFilePath[MAX_STR_LEN];
	wchar_t mName[MAX_STR_LEN];

	IDirect3DTexture9* mD3DTexture;
};


#endif