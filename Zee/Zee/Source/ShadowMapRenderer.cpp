#include "ShadowMapRenderer.h"
#include "Engine.h"
#include "Texture.h"
#include "DirectionalLightNode.h"
#include "Camera.h"
#include "Terrain.h"

LPD3DXEFFECT ShadowMapRenderer::mShadowMapEffect = NULL;
LPD3DXEFFECT ShadowMapRenderer::mShadowTexEffect = NULL;
LPD3DXEFFECT ShadowMapRenderer::mGaussianBlurEffect = NULL;

IDirect3DSurface9* ShadowMapRenderer::mShadowMapRTSurface = NULL;
Texture* ShadowMapRenderer::mShadowMapTex = NULL;
Texture* ShadowMapRenderer::mShadowTex = NULL;
IDirect3DSurface9* ShadowMapRenderer::mDepthStencilSurface = NULL;

IDirect3DVertexBuffer9* ShadowMapRenderer::mBlurQuadVB = NULL;
Texture* ShadowMapRenderer::mShadowMapBluredTexH = NULL;
Texture* ShadowMapRenderer::mShadowMapBluredTex = NULL;

VirtualLightCamera ShadowMapRenderer::mVirtualCamera;

LPDIRECT3DSURFACE9 ShadowMapRenderer::mPrevRenderTarget = NULL;
LPDIRECT3DSURFACE9 ShadowMapRenderer::mPrevDepthStencil = NULL;

void ShadowMapRenderer::createEffect()
{
	SAFE_RELEASE(mShadowMapEffect);
	SAFE_RELEASE(mShadowTexEffect);
	SAFE_RELEASE(mGaussianBlurEffect);

	IDirect3DDevice9* d3dDevice = gEngine->GetDriver()->GetD3DDevice();
	D3DXCreateEffectFromFile(d3dDevice, L"./Source/Shaders/ShadowMap.fx", NULL, NULL, 
		D3DXSHADER_DEBUG, UtilityShader::pool, &mShadowMapEffect, NULL);

	D3DXCreateEffectFromFile(d3dDevice, L"./Source/Shaders/ShadowTex.fx", NULL, NULL, 
		D3DXSHADER_DEBUG, UtilityShader::pool, &mShadowTexEffect, NULL);

	D3DXCreateEffectFromFile(d3dDevice, L"./Source/Shaders/GaussianBlur.fx", NULL, NULL, 
		D3DXSHADER_DEBUG, UtilityShader::pool, &mGaussianBlurEffect, NULL);

	_Assert(NULL != mShadowMapEffect);
	_Assert(NULL != mShadowTexEffect);
	_Assert(NULL != mGaussianBlurEffect);
}

void ShadowMapRenderer::OnLostDevice()
{
	mShadowMapEffect->OnLostDevice();
	mShadowTexEffect->OnLostDevice();
	mGaussianBlurEffect->OnLostDevice();

	SAFE_RELEASE(mBlurQuadVB);
}

void ShadowMapRenderer::OnResetDevice()
{
	mShadowMapEffect->OnResetDevice();
	mShadowTexEffect->OnResetDevice();
	mGaussianBlurEffect->OnResetDevice();

	createGaussianBlurQuadVB();
}

void ShadowMapRenderer::Init()
{
	createEffect();
	createGaussianBlurQuadVB();

	gEngine->GetDriver()->GetD3DDevice()->CreateRenderTarget(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, D3DFMT_G32R32F, 
		D3DMULTISAMPLE_8_SAMPLES, 0, false, &mShadowMapRTSurface, NULL);

	mShadowMapTex = new Texture;
	mShadowMapTex->Create(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, D3DFMT_G32R32F, D3DUSAGE_RENDERTARGET);

	mShadowMapBluredTexH = new Texture;
	mShadowMapBluredTex = new Texture;
	mShadowMapBluredTexH->Create(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, D3DFMT_G32R32F, D3DUSAGE_RENDERTARGET);
	mShadowMapBluredTex->Create(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, D3DFMT_G32R32F, D3DUSAGE_RENDERTARGET);

	mShadowTex = new Texture;
	mShadowTex->Create(SHADOW_TEX_SIZE, SHADOW_TEX_SIZE, D3DFMT_A8R8G8B8, D3DUSAGE_RENDERTARGET);

	gEngine->GetDriver()->GetD3DDevice()->CreateDepthStencilSurface(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, D3DFMT_D24X8, 
		D3DMULTISAMPLE_8_SAMPLES, 0, true, &mDepthStencilSurface, NULL);

	_Assert(mShadowMapTex != NULL);
	_Assert(mShadowTex != NULL);
	_Assert(mDepthStencilSurface != NULL);
}

void ShadowMapRenderer::Destory()
{
	SAFE_DROP(mShadowMapTex);
	SAFE_DROP(mShadowMapBluredTexH);
	SAFE_DROP(mShadowMapBluredTex);
	SAFE_DROP(mShadowTex);

	SAFE_RELEASE(mShadowMapRTSurface);
	SAFE_RELEASE(mDepthStencilSurface);

	SAFE_RELEASE(mBlurQuadVB);
}

void ShadowMapRenderer::setRenderState()
{
	IDirect3DDevice9* d3dDevice = gEngine->GetDriver()->GetD3DDevice();

	d3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	d3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);

	d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);

	d3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	d3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void ShadowMapRenderer::BeginShadowMapPass()
{
	IDirect3DDevice9* d3dDevice = gEngine->GetDriver()->GetD3DDevice();

	d3dDevice->GetRenderTarget(0, &mPrevRenderTarget);
	d3dDevice->GetDepthStencilSurface(&mPrevDepthStencil);

	d3dDevice->SetRenderTarget(0, mShadowMapRTSurface);
	d3dDevice->SetDepthStencilSurface(mDepthStencilSurface);

	d3dDevice->Clear(0, 0, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0, 1.0f, 0);

	setRenderState();
	
	mShadowMapEffect->Begin(0, 0);
	mShadowMapEffect->BeginPass(0);
}

void ShadowMapRenderer::EndShadowMapPass()
{
	IDirect3DDevice9* d3dDevice = gEngine->GetDriver()->GetD3DDevice();

	mShadowMapEffect->EndPass();
	mShadowMapEffect->End();

	d3dDevice->SetRenderTarget(0, mPrevRenderTarget);
	d3dDevice->SetDepthStencilSurface(mPrevDepthStencil);

	IDirect3DSurface9* shadowTexSurface = NULL; 
	mShadowMapTex->GetD3DTexture()->GetSurfaceLevel(0, &shadowTexSurface);

	d3dDevice->StretchRect(mShadowMapRTSurface, NULL, shadowTexSurface, NULL, D3DTEXF_NONE);

	bool shouldSaveToFile = false;
	if(shouldSaveToFile && mShadowMapTex->GetD3DTexture() != NULL)
		D3DXSaveTextureToFile(L"E:/Temp/shadowMap.jpg", D3DXIFF_JPG, mShadowMapTex->GetD3DTexture(), NULL);
}

void ShadowMapRenderer::DrawMeshShadowMapPass(const D3DXMATRIX& matWorld, Geometry* geo)
{
	_Assert(NULL != geo);

	geo->SetVertexStream();
	geo->SetVertexDeclaration();

	mShadowMapEffect->SetTechnique("Depth");

	mShadowMapEffect->SetMatrix("matWVP", &(matWorld * mVirtualCamera.matVP));
	mShadowMapEffect->SetMatrix("matWorld", &matWorld);
	mShadowMapEffect->SetRawValue("lightPos", &(mVirtualCamera.pos), 0, sizeof(Vector3));

	mShadowMapEffect->CommitChanges();

	geo->Draw();
}

void ShadowMapRenderer::DrawTerrainShadowMapPass(Terrain* terrain)
{
	_Assert(NULL != terrain);

	mShadowMapEffect->SetTechnique("Depth");

	mShadowMapEffect->SetMatrix("matWVP", &mVirtualCamera.matVP);
	mShadowMapEffect->SetMatrix("matWorld", &IDENTITY_MATRIX);
	mShadowMapEffect->SetRawValue("lightPos", &(mVirtualCamera.pos), 0, sizeof(Vector3));

	mShadowMapEffect->CommitChanges();

	std::vector<TerrainChunk*> chunks = terrain->GetChunks();

	for(std::vector<TerrainChunk*>::iterator iter = chunks.begin(); iter != chunks.end(); ++iter)
	{
		// TODO: bound判断
		TerrainChunk* chunk = *iter;

		chunk->SetVertexStream();
		chunk->Draw();
	}
}

void ShadowMapRenderer::SetupVirtualLightCamera(DirectionalLightNode* lightNode)
{
	// 计算light视角下所需渲染的场景bound
	AABBox lightSceneBound(Vector3::Zero, 200, 80, 200);

	// 由上面得到的bound计算光源虚拟摄像机的位置和正交投影矩阵(pos, nearZ, farZ, width, height)
	DirectionalLight* dirLight = lightNode->GetDirLight();
	Vector3 lightDir = dirLight->GetDirection();

	D3DXMATRIX matLightView;
	{
		Vector3 lightPos = lightNode->GetWorldPosition();

		D3DXMATRIX matRotTranspose, matTransInverse;
		D3DXMatrixTranspose(&matRotTranspose, &(lightNode->GetWorldOrient().Matrix()));
		D3DXMatrixTranslation(&matTransInverse, -lightPos.x, -lightPos.y, -lightPos.z);
		matLightView = matTransInverse * matRotTranspose; 
	}

	AABBox lightSpaceBound;
	for(int i = 0; i < 8; ++i)
	{
		Vector3 posW = lightSceneBound.GetVertex(i);
		Vector3	posL = PosVecTransform(posW, matLightView);	

		lightSpaceBound = AABBox::CombinePoint(lightSpaceBound, posL);
	}

	float nearZ = 0.1f;
	float farZ = 0;
	float width = 0;
	float height = 0;
	{
		Vector3 centerL = lightSpaceBound.GetCenter();
		D3DXMATRIX matLightViewInv;
		D3DXMatrixInverse(&matLightViewInv, 0, &matLightView);

		Vector3 centerW = PosVecTransform(centerL, matLightViewInv);
		float lightDist = fabsf(lightSpaceBound.mMax.z - lightSpaceBound.mMin.z) / 2.0f;
		width = fabsf(lightSpaceBound.mMax.x - lightSpaceBound.mMin.x);
		height = fabsf(lightSpaceBound.mMax.y - lightSpaceBound.mMin.y);

		farZ = 2.0f * lightDist;
		mVirtualCamera.pos = centerW - lightDist * lightDir;

	}

	D3DXMATRIX matLightProj;
	D3DXMatrixOrthoLH(&matLightProj, width, height, nearZ, farZ);

	D3DXMATRIX matVirtualCameraView;
	{
		Vector3 virtualCameraPos = mVirtualCamera.pos;

		D3DXMATRIX matRotTranspose, matTransInverse;
		D3DXMatrixTranspose(&matRotTranspose, &(lightNode->GetWorldOrient().Matrix()));
		D3DXMatrixTranslation(&matTransInverse, -virtualCameraPos.x, -virtualCameraPos.y, -virtualCameraPos.z);
		matVirtualCameraView = matTransInverse * matRotTranspose; 
	}

	mVirtualCamera.matVP = matVirtualCameraView * matLightProj;
}

void ShadowMapRenderer::BeginShadowTexPass()
{
	gEngine->GetDriver()->GetD3DDevice()->GetRenderTarget(0, &mPrevRenderTarget);
	gEngine->GetDriver()->GetD3DDevice()->GetDepthStencilSurface(&mPrevDepthStencil);

	IDirect3DSurface9* shadowTexSurface = NULL; 
	mShadowTex->GetD3DTexture()->GetSurfaceLevel(0, &shadowTexSurface);

	gEngine->GetDriver()->GetD3DDevice()->SetRenderTarget(0, shadowTexSurface);
	gEngine->GetDriver()->GetD3DDevice()->SetDepthStencilSurface(mDepthStencilSurface);

	gEngine->GetDriver()->GetD3DDevice()->Clear(0, 0, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0, 1.0f, 0);

	setRenderState();

	mShadowTexEffect->Begin(0, 0);
	mShadowTexEffect->BeginPass(0);
}

void ShadowMapRenderer::EndShadowTexPass()
{
	mShadowTexEffect->EndPass();
	mShadowTexEffect->End();

	gEngine->GetDriver()->GetD3DDevice()->SetRenderTarget(0, mPrevRenderTarget);
	gEngine->GetDriver()->GetD3DDevice()->SetDepthStencilSurface(mPrevDepthStencil);

	bool shouldSaveToFile = false;
	if(shouldSaveToFile && mShadowTex->GetD3DTexture() != NULL)
		D3DXSaveTextureToFile(L"E:/Temp/shadowTex.jpg", D3DXIFF_JPG, mShadowTex->GetD3DTexture(), NULL);
}

void ShadowMapRenderer::DrawMeshShadowTexPass(const D3DXMATRIX& matWorld, Geometry* geo, Camera* camera)
{
	_Assert(NULL != geo);

	geo->SetVertexStream();
	geo->SetVertexDeclaration();

	mShadowTexEffect->SetTechnique("Shadow");

	mShadowTexEffect->SetMatrix("matWVP", &(matWorld * camera->ViewMatrix() * camera->ProjMatrix()));
	mShadowTexEffect->SetMatrix("matWorld", &matWorld);
	mShadowTexEffect->SetMatrix("matLightVP", &mVirtualCamera.matVP);
	mShadowTexEffect->SetRawValue("lightPos", &(mVirtualCamera.pos), 0, sizeof(Vector3));

	_Assert(mShadowMapTex != NULL);
	mShadowTexEffect->SetTexture("shadowMapTex", mShadowMapBluredTex->GetD3DTexture());

	mShadowTexEffect->CommitChanges();

	geo->Draw();
}

void ShadowMapRenderer::DrawTerrainShadowTexPass(Terrain* terrain, Camera* camera)
{
	_Assert(NULL != terrain);

	mShadowTexEffect->SetTechnique("Shadow");

	mShadowTexEffect->SetMatrix("matWVP", &(camera->ViewMatrix() * camera->ProjMatrix()));
	mShadowTexEffect->SetMatrix("matWorld", &IDENTITY_MATRIX);
	mShadowTexEffect->SetMatrix("matLightVP", &mVirtualCamera.matVP);
	mShadowTexEffect->SetRawValue("lightPos", &(mVirtualCamera.pos), 0, sizeof(Vector3));

	_Assert(mShadowMapTex != NULL);
	mShadowTexEffect->SetTexture("shadowMapTex", mShadowMapBluredTex->GetD3DTexture());

	mShadowTexEffect->CommitChanges();

	std::vector<TerrainChunk*> chunks = terrain->GetChunks();

	for(std::vector<TerrainChunk*>::iterator iter = chunks.begin(); iter != chunks.end(); ++iter)
	{
		// TODO: bound判断
		TerrainChunk* chunk = *iter;

		chunk->SetVertexStream();
		chunk->Draw();
	}
}


Texture* ShadowMapRenderer::GetShadowTex()
{
	return mShadowTex;
}

void ShadowMapRenderer::createGaussianBlurQuadVB()
{
	SAFE_RELEASE(mBlurQuadVB);

	VertexXYZRHWUV quadVerts[4];
	quadVerts[0] = VertexXYZRHWUV(0, 0, 0, 1, 0, 0); 
	quadVerts[1] = VertexXYZRHWUV(SHADOW_MAP_SIZE, 0, 0, 1, 1, 0); 
	quadVerts[2] = VertexXYZRHWUV(0, SHADOW_MAP_SIZE, 0, 1, 0, 1); 
	quadVerts[3] = VertexXYZRHWUV(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, 0, 1, 1, 1);

	CreateVB(gEngine->GetDriver()->GetD3DDevice(), &mBlurQuadVB, quadVerts, 4, XYZRHW_UV);
}

void ShadowMapRenderer::ShadowMapGaussianBlur()
{
	IDirect3DDevice9* d3dDevice = gEngine->GetDriver()->GetD3DDevice();

	d3dDevice->GetRenderTarget(0, &mPrevRenderTarget);
	d3dDevice->GetDepthStencilSurface(&mPrevDepthStencil);

	setRenderState();

	shadowMapGaussianBlurH();
	shadowMapGaussianBlurV();

	d3dDevice->SetRenderTarget(0, mPrevRenderTarget);
	d3dDevice->SetDepthStencilSurface(mPrevDepthStencil);
}

void ShadowMapRenderer::shadowMapGaussianBlurH()
{
	IDirect3DDevice9* d3dDevice = gEngine->GetDriver()->GetD3DDevice();

	IDirect3DSurface9* texSurface = NULL; 
	mShadowMapBluredTexH->GetD3DTexture()->GetSurfaceLevel(0, &texSurface);

	d3dDevice->SetRenderTarget(0, texSurface);
	d3dDevice->SetDepthStencilSurface(mDepthStencilSurface);

	d3dDevice->Clear(0, 0, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0, 1.0f, 0);

	mGaussianBlurEffect->Begin(0, 0);
	mGaussianBlurEffect->BeginPass(0);

	d3dDevice->SetStreamSource(0, mBlurQuadVB, 0, SizeofVertex(XYZRHW_UV));
	d3dDevice->SetFVF(VertexXYZRHWUV::FVF);

	mGaussianBlurEffect->SetTechnique("GaussianBlurH");
	mGaussianBlurEffect->SetTexture("sourceTex", mShadowMapTex->GetD3DTexture());
	mGaussianBlurEffect->SetInt("sourceTexSize", SHADOW_MAP_SIZE);
	mGaussianBlurEffect->CommitChanges();

	d3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	mGaussianBlurEffect->EndPass();
	mGaussianBlurEffect->End();
}

void ShadowMapRenderer::shadowMapGaussianBlurV()
{
	IDirect3DDevice9* d3dDevice = gEngine->GetDriver()->GetD3DDevice();

	IDirect3DSurface9* texSurface = NULL; 
	mShadowMapBluredTex->GetD3DTexture()->GetSurfaceLevel(0, &texSurface);

	d3dDevice->SetRenderTarget(0, texSurface);
	d3dDevice->SetDepthStencilSurface(mDepthStencilSurface);

	d3dDevice->Clear(0, 0, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0, 1.0f, 0);

	mGaussianBlurEffect->Begin(0, 0);
	mGaussianBlurEffect->BeginPass(0);

	d3dDevice->SetStreamSource(0, mBlurQuadVB, 0, SizeofVertex(XYZRHW_UV));
	d3dDevice->SetFVF(VertexXYZRHWUV::FVF);

	mGaussianBlurEffect->SetTechnique("GaussianBlurV");
	mGaussianBlurEffect->SetTexture("sourceTex", mShadowMapBluredTexH->GetD3DTexture());
	mGaussianBlurEffect->SetInt("sourceTexSize", SHADOW_MAP_SIZE);
	mGaussianBlurEffect->CommitChanges();

	d3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	mGaussianBlurEffect->EndPass();
	mGaussianBlurEffect->End();
}

