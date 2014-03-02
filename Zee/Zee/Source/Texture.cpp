#include "Texture.h"
#include "Engine.h"

Texture::Texture()
:mD3DTexture(NULL)
{
	YString::Empty(mFilePath);
	YString::Copy(mName, _countof(mName), L"null");
}

Texture::~Texture()
{
	SAFE_RELEASE(mD3DTexture);
}

wchar_t* Texture::GetFilePath()
{
	return mFilePath;
}

IDirect3DTexture9* Texture::GetD3DTexture()
{
	return mD3DTexture;
}

void Texture::CreateFromFile(const wchar_t* filePath)
{
	_Assert(NULL == mD3DTexture);
	D3DXCreateTextureFromFile(gEngine->GetDriver()->GetD3DDevice(), filePath, &mD3DTexture);

	_Assert(NULL != mD3DTexture);

	YString::Copy(mFilePath, _countof(mFilePath), filePath);
	YString::GetFileName(mName, _countof(mName), filePath, false);
}

wchar_t* Texture::GetName()
{
	return mName;
}

void Texture::Create(int width, int height, D3DFORMAT format, DWORD usage)
{
	_Assert(NULL == mD3DTexture);
	D3DXCreateTexture(gEngine->GetDriver()->GetD3DDevice(), width, height, 1, usage, format, D3DPOOL_DEFAULT, &mD3DTexture);

	_Assert(NULL != mD3DTexture);
}
