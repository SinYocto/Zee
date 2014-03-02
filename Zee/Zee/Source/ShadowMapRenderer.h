#ifndef SHADOWMAP_RENDERER
#define SHADOWMAP_RENDERER

#include "D3DUtility.h"

#define SHADOW_MAP_SIZE 1024

class Geometry;
class Material;
class Texture;
class Camera;
class DirectionalLightNode;

struct VirtualLightCamera
{
	Vector3 pos;
	D3DXMATRIX matVP;
};

class ShadowMapRenderer
{
public:
	static void Init();

	static void OnLostDevice();
	static void OnResetDevice();

	static void Begin();
	static void End();

	static void SetupVirtualLightCamera(DirectionalLightNode* lightNode);
	static void DrawMesh(const D3DXMATRIX& matWorld, Geometry* geo);

private:
	static void createEffect();
	static void setRenderState();



private:
	static LPD3DXEFFECT mEffect;

	static Texture* mShadowMapTex;
	static IDirect3DSurface9* mDepthStencilSurface;

	static VirtualLightCamera mVirtualCamera;

	static IDirect3DSurface9* mPrevRenderTarget;
	static IDirect3DSurface9* mPrevDepthStencil;
};


#endif