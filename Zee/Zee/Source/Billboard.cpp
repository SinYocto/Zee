#include "Billboard.h"
#include "Camera.h"
#include "Engine.h"

LPD3DXEFFECT Billboard::effect = NULL;


void Billboard::OnLostDevice()
{
	mNeedResetEffect = true;

	effect->OnLostDevice();
	SAFE_RELEASE(mVertexBuffer);
}

void Billboard::OnResetDevice()
{
	if(mNeedResetEffect)
	{
		effect->OnResetDevice();
		mNeedResetEffect = false;
	}

	createVertexBuffer();
}

void Billboard::createEffect()
{
	D3DXCreateEffectFromFile(gEngine->GetDriver()->GetD3DDevice(), L"./Source/Shaders/Billboard.fx", NULL, NULL, 
		D3DXSHADER_DEBUG, NULL, &effect, NULL);

	_Assert(NULL != effect);
}

void Billboard::createVertexBuffer()
{
	VertexUV verts[4];
	verts[0] = VertexUV(-0.5f,  0.5f, 0, 0, 0);
	verts[1] = VertexUV( 0.5f,  0.5f, 0, 1, 0);
	verts[2] = VertexUV( 0.5f, -0.5f, 0, 1, 1);
	verts[3] = VertexUV(-0.5f, -0.5f, 0, 0, 1);

	CreateVB(gEngine->GetDriver()->GetD3DDevice(), &mVertexBuffer, (void*)verts, 4, XYZ_UV);
}

float Billboard::GetWidth()
{
	return mWidth;
}

float Billboard::GetHeight()
{
	return mHeight;
}

void Billboard::SetTexture(const wchar_t* texFileName)
{
	if(mTexture)
		SAFE_RELEASE(mTexture);

	D3DXCreateTextureFromFile(gEngine->GetDriver()->GetD3DDevice(), texFileName, &mTexture);	
}

void Billboard::SetColor(D3DXCOLOR color)
{
	mColor = color;
}

void Billboard::Draw(const Vector3& pos, Camera* camera)
{
	IDirect3DDevice9* d3dDevice = gEngine->GetDriver()->GetD3DDevice();

	D3DXMATRIX matScale, matRot, matTrans, matWVP;

	D3DXMatrixScaling(&matScale, mWidth, mHeight, 1.0f);
	D3DXMatrixTranslation(&matTrans, pos.x, pos.y, pos.z);

	Vector3 cameraRight = camera->GetWorldRight();
	Vector3 cameraUp = camera->GetWorldUp();
	Vector3 cameraForward = camera->GetWorldForward();

	matRot = D3DXMATRIX(cameraRight.x, cameraRight.y, cameraRight.z, 0,
		cameraUp.x, cameraUp.y, cameraUp.z, 0,
		cameraForward.x, cameraForward.y, cameraForward.z, 0,
		0, 0, 0, 1);

	matWVP = matScale * matRot * matTrans * camera->ViewMatrix() * camera->ProjMatrix();

	d3dDevice->SetStreamSource(0, mVertexBuffer, 0, SizeofVertex(XYZ_UV));
	d3dDevice->SetFVF(VertexUV::FVF);

	//d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);	
	//d3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	//d3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	effect->SetTechnique("Billboard");
	effect->SetMatrix("matWVP", &matWVP);
	effect->SetRawValue("mtlColor", &(D3DXCOLOR(mColor)), 0, sizeof(D3DXCOLOR));
	effect->SetTexture("colorTex", mTexture);
	effect->SetBool("isAlphaFromColor", true);	

	effect->CommitChanges();

	//effect->Begin(0, 0);
	//effect->BeginPass(0);

	d3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);

	//effect->EndPass();
	//effect->End();

	//d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);	
}
