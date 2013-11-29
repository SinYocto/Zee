#include "Renderer.h"
#include "Material.h"
#include "Engine.h"
#include "Billboard.h"

void Renderer::SetRenderState(ShadingMethod method)
{
	IDirect3DDevice9* d3dDevice = gEngine->GetDriver()->GetD3DDevice();

	switch(method)
	{
	case Flat:
	case View:
	case Diffuse:
	case Specular:
	case BumpSpecular:
		{
			d3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
			d3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);

			d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);

			d3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
			d3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
			break;
		}
	case BillboardMethod:
		{
			d3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
			d3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);

			d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);	
			d3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			d3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

			d3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
			d3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
			break;
		}
	case WireFrame:
		{
			d3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
			d3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);

			d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);

			d3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
			d3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
			break;
		}
	default:
		_Assert(false);
	}
}

void Renderer::DrawMesh(const D3DXMATRIX& matWorld, Geometry* geo, Material* material, Camera* camera)
{
	_Assert(NULL != material);
	_Assert(NULL != geo);

	material->Render(matWorld, geo, camera);
}

void Renderer::DrawBillboard(const Vector3& pos, Billboard* billborad, Camera* camera)
{
	_Assert(NULL != billborad);
	billborad->Draw(pos, camera);
}

void Renderer::Begin(ShadingMethod method)
{
	switch(method)
	{
	case Flat:
		SetRenderState(method);
		FlatShader::mEffect->Begin(0, 0);
		FlatShader::mEffect->BeginPass(0);
		break;

	case View:
		SetRenderState(method);
		ViewShader::mEffect->Begin(0, 0);
		ViewShader::mEffect->BeginPass(0);
		break;

	case Diffuse:
		SetRenderState(method);
		DiffuseShader::mEffect->Begin(0, 0);
		DiffuseShader::mEffect->BeginPass(0);
		break;

	case Specular:
		SetRenderState(method);
		SpecularShader::mEffect->Begin(0, 0);
		SpecularShader::mEffect->BeginPass(0);
		break;

	case BumpSpecular:
		SetRenderState(method);
		BumpSpecularShader::mEffect->Begin(0, 0);
		BumpSpecularShader::mEffect->BeginPass(0);
		break;

	case BillboardMethod:
		SetRenderState(method);
		Billboard::effect->Begin(0, 0);
		Billboard::effect->BeginPass(0);
		break;

	case WireFrame:
		SetRenderState(method);
		FlatShader::mEffect->Begin(0, 0);
		FlatShader::mEffect->BeginPass(0);
		break;

	default:
		_Assert(false);
	}
}

void Renderer::End(ShadingMethod method)
{
	switch(method)
	{
	case Flat:
		FlatShader::mEffect->EndPass();
		FlatShader::mEffect->End();
		break;

	case View:
		ViewShader::mEffect->EndPass();
		ViewShader::mEffect->End();
		break;

	case Diffuse:
		DiffuseShader::mEffect->EndPass();
		DiffuseShader::mEffect->End();
		break;

	case Specular:
		SpecularShader::mEffect->EndPass();
		SpecularShader::mEffect->End();
		break;

	case BumpSpecular:
		BumpSpecularShader::mEffect->EndPass();
		BumpSpecularShader::mEffect->End();
		break;

	case BillboardMethod:
		Billboard::effect->EndPass();
		Billboard::effect->End();
		break;

	case WireFrame:
		FlatShader::mEffect->EndPass();
		FlatShader::mEffect->End();
		break;

	default:
		_Assert(false);
	}
}
