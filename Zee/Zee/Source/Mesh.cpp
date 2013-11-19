#include "Mesh.h"
#include "Engine.h"

void Mesh::Draw(const D3DXMATRIX& matWorld, Camera* camera, bool isSolid)
{
	if(NULL == mGeo || NULL == mMaterial)
	{
		return;		// 允许构造Mesh对象时不指定geo和material对象, 此时调用draw不做任何事
	}

	_Assert(NULL != camera);

	IDirect3DDevice9* d3dDevice = gEngine->GetDriver()->GetD3DDevice();

	if(isSolid)
		d3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	else
		d3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	mMaterial->Render(matWorld, mGeo, camera);

	d3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}

void Mesh::SetMaterial(Material* mtl)
{
	if(mtl)
	{
		SAFE_DROP(mMaterial);
		mMaterial = mtl;
		mMaterial->Grab();
	}
}

Geometry* Mesh::GetGeometry()
{
	return mGeo;
}
