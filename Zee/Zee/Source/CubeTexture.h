#ifndef CUBETEXTURE_H
#define CUBETEXTURE_H

#include "D3DUtility.h"
#include "IReferenceCounted.h"

class CubeTexture : public IReferenceCounted
{
	enum
	{
		POSITIVE_X = 0,
		NEGATIVE_X = 1,
		POSITIVE_Y = 2,
		NEGATIVE_Y = 3,
		POSITIVE_Z = 4,
		NEGATIVE_Z = 5,
		SIDE_COUNTS = 6
	};

public:
	CubeTexture(const wchar_t* name);
	~CubeTexture();

	void OnLostDevice();
	void OnResetDevice();

	void CreateFromFile(const wchar_t* filePathPX, const wchar_t* filePathNX, 
		const wchar_t* filePathPY, const wchar_t* filePathNY, 
		const wchar_t* filePathPZ, const wchar_t* filePathNZ);

	wchar_t* GetName();
	IDirect3DCubeTexture9* GetD3DCubeTexture();

private:
	wchar_t mName[MAX_STR_LEN];
	wchar_t mFilePath[SIDE_COUNTS][MAX_STR_LEN];

	IDirect3DCubeTexture9* mD3DCubeTexture;
};


#endif