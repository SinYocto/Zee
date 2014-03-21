#include "ShadowMapRenderer.h"
#include "Engine.h"
#include "Texture.h"
#include "DirectionalLightNode.h"
#include "Camera.h"
#include "Terrain.h"
#include "Input.h"

LPD3DXEFFECT ShadowMapRenderer::mShadowMapEffect = NULL;
LPD3DXEFFECT ShadowMapRenderer::mShadowTexEffect = NULL;
LPD3DXEFFECT ShadowMapRenderer::mGaussianBlurEffect = NULL;

IDirect3DSurface9* ShadowMapRenderer::mShadowMapRTSurface = NULL;
Texture* ShadowMapRenderer::mShadowMapTex = NULL;
Texture* ShadowMapRenderer::mShadowTex = NULL;
IDirect3DSurface9* ShadowMapRenderer::mDepthStencilSurface = NULL;

IDirect3DVertexBuffer9* ShadowMapRenderer::mBlurQuadVB = NULL;
Texture* ShadowMapRenderer::mShadowMapBluredTexH = NULL;
Texture* ShadowMapRenderer::mShadowMapBluredTex[CASCADE_COUNTS];

VirtualLightCamera ShadowMapRenderer::mVirtualCamera[CASCADE_COUNTS];

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
	if(mShadowMapEffect)
		mShadowMapEffect->OnLostDevice();

	if(mShadowTexEffect)
		mShadowTexEffect->OnLostDevice();

	if(mGaussianBlurEffect)
		mGaussianBlurEffect->OnLostDevice();

	SAFE_RELEASE(mBlurQuadVB);
	SAFE_RELEASE(mShadowMapRTSurface);
	SAFE_RELEASE(mDepthStencilSurface);

	SAFE_DELETE(mShadowMapTex);
	SAFE_DELETE(mShadowMapBluredTexH);
	SAFE_DELETE(mShadowTex);
	for(int i = 0; i < CASCADE_COUNTS; ++i)
		SAFE_DELETE(mShadowMapBluredTex[i]);
}

void ShadowMapRenderer::OnResetDevice()
{
	if(mShadowMapEffect)
		mShadowMapEffect->OnResetDevice();

	if(mShadowTexEffect)
		mShadowTexEffect->OnResetDevice();

	if(mGaussianBlurEffect)
		mGaussianBlurEffect->OnResetDevice();

	createGaussianBlurQuadVB();
	createRendertargetDepthStencil();
	createTextures();
}

void ShadowMapRenderer::Init()
{
	createEffect();
	createGaussianBlurQuadVB();
	createRendertargetDepthStencil();
	createTextures();
}

void ShadowMapRenderer::createTextures()
{
	SAFE_DELETE(mShadowMapTex);
	SAFE_DELETE(mShadowMapBluredTexH);
	SAFE_DELETE(mShadowTex);
	for(int i = 0; i < CASCADE_COUNTS; ++i)
		SAFE_DELETE(mShadowMapBluredTex[i]);

	mShadowMapTex = New Texture;
	mShadowMapTex->Create(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, D3DFMT_G32R32F, D3DUSAGE_RENDERTARGET);

	mShadowMapBluredTexH = New Texture;
	mShadowMapBluredTexH->Create(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, D3DFMT_G32R32F, D3DUSAGE_RENDERTARGET);

	for(int i = 0; i < CASCADE_COUNTS; ++i)
	{
		mShadowMapBluredTex[i] = New Texture;
		mShadowMapBluredTex[i]->Create(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, D3DFMT_G32R32F, D3DUSAGE_RENDERTARGET);

		_Assert(mShadowMapBluredTex[i] != NULL);
	}

	mShadowTex = New Texture;
	mShadowTex->Create(SHADOW_TEX_SIZE, SHADOW_TEX_SIZE, D3DFMT_A8R8G8B8, D3DUSAGE_RENDERTARGET);

	_Assert(mShadowMapTex != NULL);
	_Assert(mShadowTex != NULL);
}

void ShadowMapRenderer::Destory()
{
	for(int i = 0; i < CASCADE_COUNTS; ++i)
	{
		SAFE_DROP(mShadowMapBluredTex[i]);
	}

	SAFE_DROP(mShadowMapBluredTexH);
	SAFE_DROP(mShadowMapTex);
	SAFE_DROP(mShadowTex);

	SAFE_RELEASE(mShadowMapRTSurface);
	SAFE_RELEASE(mDepthStencilSurface);

	SAFE_RELEASE(mBlurQuadVB);
}

void ShadowMapRenderer::createRendertargetDepthStencil()
{
	SAFE_RELEASE(mShadowMapRTSurface);
	SAFE_RELEASE(mDepthStencilSurface);

	gEngine->GetDriver()->GetD3DDevice()->CreateRenderTarget(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, D3DFMT_G32R32F, 
		D3DMULTISAMPLE_8_SAMPLES, 0, false, &mShadowMapRTSurface, NULL);

	gEngine->GetDriver()->GetD3DDevice()->CreateDepthStencilSurface(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, D3DFMT_D24X8, 
		D3DMULTISAMPLE_8_SAMPLES, 0, true, &mDepthStencilSurface, NULL);

	_Assert(mShadowMapRTSurface != NULL);
	_Assert(mDepthStencilSurface != NULL);
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

	SAFE_RELEASE(mPrevRenderTarget);		// GetRenderTarget方法会增加引用计数, 需要release, 否则设备无法恢复
	SAFE_RELEASE(mPrevDepthStencil);

	IDirect3DSurface9* shadowTexSurface = NULL; 
	mShadowMapTex->GetD3DTexture()->GetSurfaceLevel(0, &shadowTexSurface);

	d3dDevice->StretchRect(mShadowMapRTSurface, NULL, shadowTexSurface, NULL, D3DTEXF_NONE);

	bool shouldSaveToFile = false;
	if(shouldSaveToFile && mShadowMapTex->GetD3DTexture() != NULL)
		D3DXSaveTextureToFile(L"E:/Temp/shadowMap.jpg", D3DXIFF_JPG, mShadowMapTex->GetD3DTexture(), NULL);

	SAFE_RELEASE(shadowTexSurface);
}

void ShadowMapRenderer::DrawMeshShadowMapPass(const D3DXMATRIX& matWorld, Geometry* geo, int cascadeIndex)
{
	_Assert(NULL != geo);

	geo->SetVertexStream();
	geo->SetVertexDeclaration();

	mShadowMapEffect->SetTechnique("Depth");

	mShadowMapEffect->SetMatrix("matWVP", &(matWorld * mVirtualCamera[cascadeIndex].matVP));
	mShadowMapEffect->SetMatrix("matWorld", &matWorld);
	mShadowMapEffect->SetRawValue("lightPos", &(mVirtualCamera[cascadeIndex].pos), 0, sizeof(Vector3));

	mShadowMapEffect->CommitChanges();

	geo->Draw();
}

void ShadowMapRenderer::DrawTerrainShadowMapPass(Terrain* terrain, int cascadeIndex)
{
	_Assert(NULL != terrain);

	mShadowMapEffect->SetTechnique("Depth");

	mShadowMapEffect->SetMatrix("matWVP", &mVirtualCamera[cascadeIndex].matVP);
	mShadowMapEffect->SetMatrix("matWorld", &IDENTITY_MATRIX);
	mShadowMapEffect->SetRawValue("lightPos", &(mVirtualCamera[cascadeIndex].pos), 0, sizeof(Vector3));

	mShadowMapEffect->CommitChanges();

	std::vector<TerrainChunk*> chunks = terrain->GetChunks();

	for(std::vector<TerrainChunk*>::iterator iter = chunks.begin(); iter != chunks.end(); ++iter)
	{
		TerrainChunk* chunk = *iter;

		if(!chunk->IntersectWithBBox(mVirtualCamera[cascadeIndex].bound))
			continue;

		chunk->SetVertexStream();
		chunk->Draw();
	}
}

void ShadowMapRenderer::SetupVirtualLightCameras(Camera* camera, DirectionalLightNode* lightNode)
{
	for(int i = 0; i < CASCADE_COUNTS; ++i)
	{
		setupVirtualLightCamera(camera, lightNode, i);
	}
}

void ShadowMapRenderer::setupVirtualLightCamera(Camera* camera, DirectionalLightNode* lightNode, int cascadeIndex)
{
	const float BOUND_EXPAND_FACTOR = 0.8f;

	_Assert(camera != NULL);
	_Assert(cascadeIndex >= 0 && cascadeIndex < CASCADE_COUNTS);

	DirectionalLight* dirLight = lightNode->GetDirLight();
	Vector3 lightDir = dirLight->GetDirection();
	lightDir.Normalize();

	Vector3 frustumPos[5];
	{
		float farZ = 0;
		float aspect = 0;
		float fov = 0;

		camera->GetCameraParams(NULL, &farZ, &fov, &aspect);
		farZ *= calcCascadeDist(cascadeIndex);


		float fy = farZ * tanf(fov / 2.0f);
		float fx = aspect * fy;

		frustumPos[0] = Vector3::Zero;
		frustumPos[1] = Vector3(-fx,  fy, farZ);
		frustumPos[2] = Vector3( fx,  fy, farZ);
		frustumPos[3] = Vector3(-fx, -fy, farZ);
		frustumPos[4] = Vector3( fx, -fy, farZ);
	}

	D3DXMATRIX matCameraViewInv;
	D3DXMatrixInverse(&matCameraViewInv, 0, &camera->ViewMatrix());

	D3DXMATRIX matLightView;
	{
		Vector3 lightPos = lightNode->GetWorldPosition();

		D3DXMATRIX matRotTranspose, matTransInverse;
		D3DXMatrixTranspose(&matRotTranspose, &(lightNode->GetWorldOrient().Matrix()));
		D3DXMatrixTranslation(&matTransInverse, -lightPos.x, -lightPos.y, -lightPos.z);
		matLightView = matTransInverse * matRotTranspose; 
	}

	AABBox lightSpaceBound;
	for(int i = 0; i < 5; ++i)
	{
		Vector3 posW = PosVecTransform(frustumPos[i], matCameraViewInv);
		Vector3	posL = PosVecTransform(posW, matLightView);	

		lightSpaceBound = AABBox::CombinePoint(lightSpaceBound, posL);
	}

	float nearZ = 0.1f;
	float farZ = 0;
	float width = 0;
	float height = 0;
	{
		Vector3 lookDir = camera->GetWorldForward();
		lookDir.Normalize();

		float adjustFactor = lookDir.Dot(lightDir);		// lookDir与lightDir贴近时, 向后扩展virtualCamera范围
		float expandDist = adjustFactor * BOUND_EXPAND_FACTOR * fabsf(lightSpaceBound.mMax.z - lightSpaceBound.mMin.z);

		Vector3 centerL = lightSpaceBound.GetCenter();
		D3DXMATRIX matLightViewInv;
		D3DXMatrixInverse(&matLightViewInv, 0, &matLightView);

		Vector3 centerW = PosVecTransform(centerL, matLightViewInv);
		float lightDist = fabsf(lightSpaceBound.mMax.z - lightSpaceBound.mMin.z) / 2.0f;
		width = fabsf(lightSpaceBound.mMax.x - lightSpaceBound.mMin.x);
		height = fabsf(lightSpaceBound.mMax.y - lightSpaceBound.mMin.y);

		farZ = 2.0f * lightDist + expandDist;
		mVirtualCamera[cascadeIndex].pos = centerW - (lightDist + expandDist) * lightDir;
	}

	D3DXMATRIX matLightProj;
	D3DXMatrixOrthoLH(&matLightProj, width, height, nearZ, farZ);

	D3DXMATRIX matVirtualCameraView;
	{
		Vector3 virtualCameraPos = mVirtualCamera[cascadeIndex].pos;

		D3DXMATRIX matRotTranspose, matTransInverse;
		D3DXMatrixTranspose(&matRotTranspose, &(lightNode->GetWorldOrient().Matrix()));
		D3DXMatrixTranslation(&matTransInverse, -virtualCameraPos.x, -virtualCameraPos.y, -virtualCameraPos.z);
		matVirtualCameraView = matTransInverse * matRotTranspose; 

		mVirtualCamera[cascadeIndex].bound = AABBox::MatTransform(AABBox(Vector3(0, 0, farZ/2.0f), width, height, farZ), 
			InversedMatrix(matVirtualCameraView));
	}

	mVirtualCamera[cascadeIndex].matVP = matVirtualCameraView * matLightProj;
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

	SAFE_RELEASE(shadowTexSurface);
}

void ShadowMapRenderer::EndShadowTexPass()
{
	mShadowTexEffect->EndPass();
	mShadowTexEffect->End();

	gEngine->GetDriver()->GetD3DDevice()->SetRenderTarget(0, mPrevRenderTarget);
	gEngine->GetDriver()->GetD3DDevice()->SetDepthStencilSurface(mPrevDepthStencil);

	SAFE_RELEASE(mPrevRenderTarget);		// GetRenderTarget方法会增加引用计数, 需要release, 否则设备无法恢复
	SAFE_RELEASE(mPrevDepthStencil);

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

	_Assert(CASCADE_COUNTS >= 3);
	mShadowTexEffect->SetMatrix("matLightVP0", &mVirtualCamera[0].matVP);
	mShadowTexEffect->SetMatrix("matLightVP1", &mVirtualCamera[1].matVP);
	mShadowTexEffect->SetMatrix("matLightVP2", &mVirtualCamera[2].matVP);

	mShadowTexEffect->SetRawValue("lightPos0", &(mVirtualCamera[0].pos), 0, sizeof(Vector3));
	mShadowTexEffect->SetRawValue("lightPos1", &(mVirtualCamera[1].pos), 0, sizeof(Vector3));
	mShadowTexEffect->SetRawValue("lightPos2", &(mVirtualCamera[2].pos), 0, sizeof(Vector3));

	mShadowTexEffect->SetTexture("shadowMapTex0", mShadowMapBluredTex[0]->GetD3DTexture());
	mShadowTexEffect->SetTexture("shadowMapTex1", mShadowMapBluredTex[1]->GetD3DTexture());
	mShadowTexEffect->SetTexture("shadowMapTex2", mShadowMapBluredTex[2]->GetD3DTexture());

	float farZ = 0;
	camera->GetCameraParams(NULL, &farZ, NULL, NULL);
	mShadowTexEffect->SetFloat("farZ", farZ);

	mShadowTexEffect->CommitChanges();

	geo->Draw();
}

void ShadowMapRenderer::DrawTerrainShadowTexPass(Terrain* terrain, Camera* camera)
{
	_Assert(NULL != terrain);

	mShadowTexEffect->SetTechnique("Shadow");

	mShadowTexEffect->SetMatrix("matWVP", &(camera->ViewMatrix() * camera->ProjMatrix()));
	mShadowTexEffect->SetMatrix("matWorld", &IDENTITY_MATRIX);

	_Assert(CASCADE_COUNTS >= 3);
	mShadowTexEffect->SetMatrix("matLightVP0", &mVirtualCamera[0].matVP);
	mShadowTexEffect->SetMatrix("matLightVP1", &mVirtualCamera[1].matVP);
	mShadowTexEffect->SetMatrix("matLightVP2", &mVirtualCamera[2].matVP);

	mShadowTexEffect->SetRawValue("lightPos0", &(mVirtualCamera[0].pos), 0, sizeof(Vector3));
	mShadowTexEffect->SetRawValue("lightPos1", &(mVirtualCamera[1].pos), 0, sizeof(Vector3));
	mShadowTexEffect->SetRawValue("lightPos2", &(mVirtualCamera[2].pos), 0, sizeof(Vector3));

	mShadowTexEffect->SetTexture("shadowMapTex0", mShadowMapBluredTex[0]->GetD3DTexture());
	mShadowTexEffect->SetTexture("shadowMapTex1", mShadowMapBluredTex[1]->GetD3DTexture());
	mShadowTexEffect->SetTexture("shadowMapTex2", mShadowMapBluredTex[2]->GetD3DTexture());

	float farZ = 0;
	camera->GetCameraParams(NULL, &farZ, NULL, NULL);
	mShadowTexEffect->SetFloat("farZ", farZ);

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

void ShadowMapRenderer::ShadowMapGaussianBlur(int cascadeIndex)
{
	IDirect3DDevice9* d3dDevice = gEngine->GetDriver()->GetD3DDevice();

	d3dDevice->GetRenderTarget(0, &mPrevRenderTarget);
	d3dDevice->GetDepthStencilSurface(&mPrevDepthStencil);

	setRenderState();

	shadowMapGaussianBlurH();
	shadowMapGaussianBlurV(cascadeIndex);

	d3dDevice->SetRenderTarget(0, mPrevRenderTarget);
	d3dDevice->SetDepthStencilSurface(mPrevDepthStencil);

	SAFE_RELEASE(mPrevRenderTarget);		// GetRenderTarget方法会增加引用计数, 需要release, 否则设备无法恢复
	SAFE_RELEASE(mPrevDepthStencil);

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

	SAFE_RELEASE(texSurface);
}

void ShadowMapRenderer::shadowMapGaussianBlurV(int cascadeIndex)
{
	_Assert(cascadeIndex >= 0 && cascadeIndex < CASCADE_COUNTS);

	IDirect3DDevice9* d3dDevice = gEngine->GetDriver()->GetD3DDevice();

	IDirect3DSurface9* texSurface = NULL; 
	mShadowMapBluredTex[cascadeIndex]->GetD3DTexture()->GetSurfaceLevel(0, &texSurface);

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

	SAFE_RELEASE(texSurface);
}

AABBox ShadowMapRenderer::GetVirtualCameraBound(int cascadeIndex)
{
	_Assert(cascadeIndex >= 0 && cascadeIndex < CASCADE_COUNTS);
	return mVirtualCamera[cascadeIndex].bound;
}

// 划分cascade的算法: 划分比例为1:2:4:8:...
float ShadowMapRenderer::calcCascadeDist(int cascadeIndex)
{
	_Assert(cascadeIndex >= 0 && cascadeIndex < CASCADE_COUNTS);

	float sumTotal = 0;
	float sumCurCascade = 0;
	for(int i = 0; i < CASCADE_COUNTS; ++i)
	{
		sumTotal += powf(2.0f, (float)i);

		if(i <= cascadeIndex)
			sumCurCascade += powf(2.0f, (float)i);
	}

	return sumCurCascade / sumTotal;
}
