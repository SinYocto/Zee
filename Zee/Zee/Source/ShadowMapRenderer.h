#ifndef SHADOWMAP_RENDERER
#define SHADOWMAP_RENDERER

#include "D3DUtility.h"

#define SHADOW_MAP_SIZE 1024
#define SHADOW_TEX_SIZE 1024

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

	static void BeginShadowMapPass();
	static void EndShadowMapPass();

	static void BeginShadowTexPass();
	static void EndShadowTexPass();

	static void SetupVirtualLightCamera(DirectionalLightNode* lightNode);
	static void DrawMeshShadowMapPass(const D3DXMATRIX& matWorld, Geometry* geo);
	static void DrawMeshShadowTexPass(const D3DXMATRIX& matWorld, Geometry* geo, Camera* camera);

	static Texture* GetShadowTex();

private:
	static void createEffect();
	static void setRenderState();



private:
	static LPD3DXEFFECT mShadowMapEffect;
	static LPD3DXEFFECT mShadowTexEffect;

	static Texture* mShadowMapTex;
	static Texture* mShadowTex;
	static IDirect3DSurface9* mDepthStencilSurface;

	static VirtualLightCamera mVirtualCamera;

	static IDirect3DSurface9* mPrevRenderTarget;
	static IDirect3DSurface9* mPrevDepthStencil;
};


#endif