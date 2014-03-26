#include"SkyBox.h"
#include "Engine.h"
#include "Camera.h"

LPD3DXEFFECT SkyBox::effect = NULL;
const float SkyBox::DIST = 250.0f;

SkyBox::SkyBox()
:mVertexBuffer(NULL)
,mIndexBuffer(NULL)
,mCubeTex(NULL)
{
	if(!effect)
		createEffect();

	build();
}

SkyBox::~SkyBox()
{
	SAFE_RELEASE(mVertexBuffer);
	SAFE_RELEASE(mIndexBuffer);

	SAFE_DROP(mCubeTex);
}

void SkyBox::OnLostDevice()
{
	effect->OnLostDevice();

	SAFE_RELEASE(mVertexBuffer);
	SAFE_RELEASE(mIndexBuffer);
}

void SkyBox::OnResetDevice()
{
	effect->OnResetDevice();

	build();
}

void SkyBox::createEffect()
{
	SAFE_RELEASE(effect);

	D3DXCreateEffectFromFile(gEngine->GetDriver()->GetD3DDevice(), L"./Source/Shaders/SkyBox.fx", NULL, NULL, 
		D3DXSHADER_DEBUG, UtilityShader::pool, &effect, NULL);

	_Assert(NULL != effect);
}

void SkyBox::SetCubeTex(const wchar_t* cubeTexName)
{
	SAFE_DROP(mCubeTex);

	mCubeTex = gEngine->GetTextureManger()->GetCubeTexture(cubeTexName);
	mCubeTex->Grab();
}

void SkyBox::SetCubeTex(CubeTexture* cubeTex)
{
	_Assert(NULL != cubeTex);

	SAFE_DROP(mCubeTex); 
	mCubeTex = cubeTex;
	mCubeTex->Grab();
}

void SkyBox::build()
{
	_Assert(NUM_VERTICES == 24);
	_Assert(NUM_TRIES == 12);

	Vector3* pos = New Vector3[24];
	pos[0] = DIST * Vector3(-1.0f, 1.0f,  1.0f);
	pos[1] = DIST * Vector3( 1.0f, 1.0f,  1.0f);
	pos[2] = DIST * Vector3(-1.0f, 1.0f, -1.0f);
	pos[3] = DIST * Vector3( 1.0f, 1.0f, -1.0f);

	pos[4] = DIST * Vector3( 1.0f, -1.0f,  1.0f);
	pos[5] = DIST * Vector3(-1.0f, -1.0f,  1.0f);
	pos[6] = DIST * Vector3( 1.0f, -1.0f, -1.0f);
	pos[7] = DIST * Vector3(-1.0f, -1.0f, -1.0f);

	pos[8]  = DIST * Vector3( 1.0f,  1.0f, 1.0f);
	pos[9]  = DIST * Vector3(-1.0f,  1.0f, 1.0f);
	pos[10] = DIST * Vector3( 1.0f, -1.0f, 1.0f);
	pos[11] = DIST * Vector3(-1.0f, -1.0f, 1.0f);

	pos[12] = DIST * Vector3(-1.0f,  1.0f, -1.0f);
	pos[13] = DIST * Vector3( 1.0f,  1.0f, -1.0f);
	pos[14] = DIST * Vector3(-1.0f, -1.0f, -1.0f);
	pos[15] = DIST * Vector3( 1.0f, -1.0f, -1.0f);

	pos[16] = DIST * Vector3(-1.0f,  1.0f,  1.0f);
	pos[17] = DIST * Vector3(-1.0f,  1.0f, -1.0f);
	pos[18] = DIST * Vector3(-1.0f, -1.0f,  1.0f);
	pos[19] = DIST * Vector3(-1.0f, -1.0f, -1.0f);

	pos[20] = DIST * Vector3( 1.0f,  1.0f, -1.0f);
	pos[21] = DIST * Vector3( 1.0f,  1.0f,  1.0f);
	pos[22] = DIST * Vector3( 1.0f, -1.0f, -1.0f);
	pos[23] = DIST * Vector3( 1.0f, -1.0f,  1.0f);

	Vector2 *uv = New Vector2[24];
	uv[0] = Vector2(0, 0);
	uv[1] = Vector2(1, 0);
	uv[2] = Vector2(0, 1);
	uv[3] = Vector2(1, 1);

	uv[4] = Vector2(0, 0);
	uv[5] = Vector2(1, 0);
	uv[6] = Vector2(0, 1);
	uv[7] = Vector2(1, 1);

	uv[8]  = Vector2(0, 0);
	uv[9]  = Vector2(1, 0);
	uv[10] = Vector2(0, 1);
	uv[11] = Vector2(1, 1);

	uv[12] = Vector2(0, 0);
	uv[13] = Vector2(1, 0);
	uv[14] = Vector2(0, 1);
	uv[15] = Vector2(1, 1);

	uv[16] = Vector2(0, 0);
	uv[17] = Vector2(1, 0);
	uv[18] = Vector2(0, 1);
	uv[19] = Vector2(1, 1);

	uv[20] = Vector2(0, 0);
	uv[21] = Vector2(1, 0);
	uv[22] = Vector2(0, 1);
	uv[23] = Vector2(1, 1);

	DWORD* indices = New DWORD[36];
	for(int i = 0; i < 6; i++)
	{
		indices[6*i + 0] = 4*i;
		indices[6*i + 1] = 4*i + 2;
		indices[6*i + 2] = 4*i + 1;
			
		indices[6*i + 3] = 4*i + 2;
		indices[6*i + 4] = 4*i + 3;
		indices[6*i + 5] = 4*i + 1;
	}
			
	void* vertexData = New VertexUV[NUM_VERTICES];
	for(int i = 0; i < NUM_VERTICES; ++i)
	{
		VertexUV vert = VertexUV(pos[i].x, pos[i].y, pos[i].z, uv[i].x, uv[i].y);
		((VertexUV*)vertexData)[i] = vert;
	}

	CreateVB(gEngine->GetDriver()->GetD3DDevice(), &mVertexBuffer, vertexData, NUM_VERTICES, XYZ_UV);
	CreateIB(gEngine->GetDriver()->GetD3DDevice(), &mIndexBuffer, indices, 3 * NUM_TRIES);

	delete[] pos;
	delete[] uv;
	delete[] indices;
	delete[] vertexData;
}

void SkyBox::Draw(Camera* camera)
{
	_Assert(NULL != camera);

	IDirect3DDevice9* d3dDevice = gEngine->GetDriver()->GetD3DDevice();

	d3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	d3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, false);

	d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);

	d3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	d3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	d3dDevice->SetStreamSource(0, mVertexBuffer, 0, SizeofVertex(XYZ_UV));
	d3dDevice->SetFVF(VertexUV::FVF);
	d3dDevice->SetIndices(mIndexBuffer);


	Vector3 centerPos = camera->GetWorldPosition();
	D3DXMATRIX matTrans;
	D3DXMatrixTranslation(&matTrans, centerPos.x, centerPos.y, centerPos.z);

	effect->SetTechnique("CubeMapping");
	effect->SetMatrix("matWVP", &(matTrans * camera->ViewMatrix() * camera->ProjMatrix()));
	effect->SetMatrix("matWorld", &matTrans);
	effect->SetTexture("cubeTex", mCubeTex->GetD3DCubeTexture());
	effect->SetRawValue("centerPos", &centerPos, 0, sizeof(Vector3));

	effect->Begin(0, 0);
	effect->BeginPass(0);

	d3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, NUM_VERTICES, 0, NUM_TRIES);
	Profiler::AddDrawCalls();
	Profiler::AddNumVerts(NUM_VERTICES);
	Profiler::AddNumTris(NUM_TRIES);

	effect->EndPass();
	effect->End();

}