#ifndef SKYBOX_H
#define SKYBOX_H

#include "D3DUtility.h"
#include "CubeTexture.h"

class Camera;

class SkyBox
{
	static const int NUM_VERTICES = 24;
	static const int NUM_TRIES = 12;
	static const float DIST;

public:
	SkyBox();
	~SkyBox();

	void OnLostDevice();
	void OnResetDevice();

	void SetCubeTex(const wchar_t* cubeTexName);
	void SetCubeTex(CubeTexture* cubeTex);

	void Draw(Camera* camera);
	void createEffect();

private:
	void build();

private:
	static LPD3DXEFFECT effect;

	IDirect3DVertexBuffer9* mVertexBuffer;
	IDirect3DIndexBuffer9*  mIndexBuffer;

	CubeTexture* mCubeTex;
};


#endif