#ifndef SHADOWMAP_RENDERER
#define SHADOWMAP_RENDERER

#include "D3DUtility.h"
#include "Bound.h"

#define SHADOW_MAP_SIZE 1024
#define SHADOW_TEX_SIZE 1024

#define CASCADE_COUNTS 3

class Geometry;
class Material;
class Texture;
class Camera;
class DirectionalLightNode;
class Terrain;

struct VirtualLightCamera
{
	Vector3 pos;
	D3DXMATRIX matVP;

	AABBox bound;
};

class ShadowMapRenderer
{
public:
	static void Init();
	static void Destory();

	static void OnLostDevice();
	static void OnResetDevice();

	static void BeginShadowMapPass();
	static void EndShadowMapPass();

	static void BeginShadowTexPass();
	static void EndShadowTexPass();

	static void SetupVirtualLightCameras(Camera* camera, DirectionalLightNode* lightNode);

	static void DrawMeshShadowMapPass(const D3DXMATRIX& matWorld, Geometry* geo, int cascadeIndex);
	static void DrawMeshShadowTexPass(const D3DXMATRIX& matWorld, Geometry* geo, Camera* camera);

	static void DrawTerrainShadowMapPass(Terrain* terrain, int cascadeIndex);
	static void DrawTerrainShadowTexPass(Terrain* terrain, Camera* camera);

	static void ShadowMapGaussianBlur(int cascadeIndex);

	static Texture* GetShadowTex();
	static AABBox GetVirtualCameraBound(int cascadeIndex);

private:
	static void createEffect();
	static void createGaussianBlurQuadVB();
	static void setRenderState();

	static void shadowMapGaussianBlurH();
	static void shadowMapGaussianBlurV(int cascadeIndex);

	static void setupVirtualLightCamera(Camera* camera, DirectionalLightNode* lightNode, int cascadeIndex);

private:
	static LPD3DXEFFECT mShadowMapEffect;
	static LPD3DXEFFECT mShadowTexEffect;
	static LPD3DXEFFECT mGaussianBlurEffect;

	static IDirect3DSurface9* mShadowMapRTSurface;	// 需要先使用这个surface作为启用了MSAA的rendertarget
	static Texture* mShadowMapTex;
	static Texture* mShadowTex;
	static IDirect3DSurface9* mDepthStencilSurface;

	static IDirect3DVertexBuffer9* mBlurQuadVB;
	static Texture* mShadowMapBluredTexH;
	static Texture* mShadowMapBluredTex[CASCADE_COUNTS];

	static VirtualLightCamera mVirtualCamera[CASCADE_COUNTS];

	static IDirect3DSurface9* mPrevRenderTarget;
	static IDirect3DSurface9* mPrevDepthStencil;
};


#endif