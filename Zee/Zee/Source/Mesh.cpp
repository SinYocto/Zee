#include "Mesh.h"

void Mesh::Draw(const D3DXMATRIX& matWorld, Camera* camera, bool isSolid)
{
	if(NULL == mGeo || NULL == mMaterial)
	{
		return;		// ������Mesh����ʱ��ָ��geo��material����, ��ʱ����draw�����κ���
	}

	_Assert(NULL != camera);

	if(isSolid)
		Driver::D3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	else
		Driver::D3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	mMaterial->Render(matWorld, mGeo, camera);

	Driver::D3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
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
