#include "Mesh.h"
#include "DebugDrawer.h"

void Mesh::Draw(Camera* camera)
{
	if(NULL == mGeo || NULL == mMaterial)
	{
		return;		// ������Mesh����ʱ��ָ��geo��material����, ��ʱ����draw�����κ���
	}

	_Assert(NULL != camera);

	switch(mAttribute.displayMode)
	{
	case WIRE_FRAME:
		Driver::D3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		break;
	case SOLID:
		Driver::D3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		break;
	}

	mMaterial->Render(this, mGeo, camera);

	Driver::D3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	if(mAttribute.drawBBox && mAABBox.isValid())
	{
		DebugDrawer::DrawAABBox(mAABBox, 0xffff0000, camera);
	}
}

void Mesh::calCurrentAABBox()
{
	_Assert(NULL != mGeo);
	mGeo->CalcDynamicAABBox(mWorldPos, mWorldOrient, &mAABBox);
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
