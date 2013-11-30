#include "Texture.h"
#include "Engine.h"

Texture::Texture()
:mD3DTexture(NULL)
{

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
	YString::Copy(mName, _countof(mName), filePath);		// TODO: 获取路径中的文件名
}