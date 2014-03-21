#include "CubeTexture.h"
#include "Engine.h"

CubeTexture::CubeTexture(const wchar_t* name)
:mD3DCubeTexture(NULL)
{
	YString::Copy(mName, _countof(mName), name);

	for(int i = 0; i < SIDE_COUNTS; ++i)
		YString::Empty(mFilePath[i]);
}

CubeTexture::~CubeTexture()
{
	SAFE_RELEASE(mD3DCubeTexture);
}

void CubeTexture::OnLostDevice()
{
	SAFE_RELEASE(mD3DCubeTexture);
}

void CubeTexture::OnResetDevice()
{
	if(!YString::isEmpty(mFilePath[0]))
	{
		CreateFromFile(mFilePath[0], mFilePath[1], mFilePath[2], mFilePath[3], mFilePath[4], mFilePath[5]);
	}
}

IDirect3DCubeTexture9* CubeTexture::GetD3DCubeTexture()
{
	return mD3DCubeTexture;
}

void CubeTexture::CreateFromFile(const wchar_t* filePathPX, const wchar_t* filePathNX, 
								 const wchar_t* filePathPY, const wchar_t* filePathNY, 
								 const wchar_t* filePathPZ, const wchar_t* filePathNZ)
{
	const int CUBE_TEX_SIZE = 512;

	_Assert(NULL == mD3DCubeTexture);

	gEngine->GetDriver()->GetD3DDevice()->CreateCubeTexture(CUBE_TEX_SIZE, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, 
		&mD3DCubeTexture, NULL);
	_Assert(NULL != mD3DCubeTexture);

	YString::Copy(mFilePath[POSITIVE_X], _countof(mFilePath[POSITIVE_X]), filePathPX);
	YString::Copy(mFilePath[NEGATIVE_X], _countof(mFilePath[NEGATIVE_X]), filePathNX);
	YString::Copy(mFilePath[POSITIVE_Y], _countof(mFilePath[POSITIVE_Y]), filePathPY);
	YString::Copy(mFilePath[NEGATIVE_Y], _countof(mFilePath[NEGATIVE_Y]), filePathNY);
	YString::Copy(mFilePath[POSITIVE_Z], _countof(mFilePath[POSITIVE_Z]), filePathPZ);
	YString::Copy(mFilePath[NEGATIVE_Z], _countof(mFilePath[NEGATIVE_Z]), filePathNZ);

	IDirect3DSurface9 *cubeTexSurf;

	mD3DCubeTexture->GetCubeMapSurface(D3DCUBEMAP_FACE_POSITIVE_X, 0, &cubeTexSurf);
	D3DXLoadSurfaceFromFile(cubeTexSurf, NULL, NULL, filePathPX, NULL, D3DX_DEFAULT, 0, NULL);
	SAFE_RELEASE(cubeTexSurf);

	mD3DCubeTexture->GetCubeMapSurface(D3DCUBEMAP_FACE_NEGATIVE_X, 0, &cubeTexSurf);
	D3DXLoadSurfaceFromFile(cubeTexSurf, NULL, NULL, filePathNX, NULL, D3DX_DEFAULT, 0, NULL);
	SAFE_RELEASE(cubeTexSurf);

	mD3DCubeTexture->GetCubeMapSurface(D3DCUBEMAP_FACE_POSITIVE_Y, 0, &cubeTexSurf);
	D3DXLoadSurfaceFromFile(cubeTexSurf, NULL, NULL, filePathPY, NULL, D3DX_DEFAULT, 0, NULL);
	SAFE_RELEASE(cubeTexSurf);

	mD3DCubeTexture->GetCubeMapSurface(D3DCUBEMAP_FACE_NEGATIVE_Y, 0, &cubeTexSurf);
	D3DXLoadSurfaceFromFile(cubeTexSurf, NULL, NULL, filePathNY, NULL, D3DX_DEFAULT, 0, NULL);
	SAFE_RELEASE(cubeTexSurf);

	mD3DCubeTexture->GetCubeMapSurface(D3DCUBEMAP_FACE_POSITIVE_Z, 0, &cubeTexSurf);
	D3DXLoadSurfaceFromFile(cubeTexSurf, NULL, NULL, filePathPZ, NULL, D3DX_DEFAULT, 0, NULL);
	SAFE_RELEASE(cubeTexSurf);

	mD3DCubeTexture->GetCubeMapSurface(D3DCUBEMAP_FACE_NEGATIVE_Z, 0, &cubeTexSurf);
	D3DXLoadSurfaceFromFile(cubeTexSurf, NULL, NULL, filePathNZ, NULL, D3DX_DEFAULT, 0, NULL);
	SAFE_RELEASE(cubeTexSurf);
}

wchar_t* CubeTexture::GetName()
{
	return mName;
}