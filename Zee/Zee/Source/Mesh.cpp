#include "Mesh.h"

void Mesh::Draw(Camera* camera)
{
	if(NULL == mGeo || NULL == mMaterial)
	{
		return;		// ������Mesh����ʱ��ָ��geo��material����, ��ʱ����draw�����κ���
	}

	_Assert(NULL != camera);

	switch(mDisplayMode)
	{
	case WIRE_FRAME:
		Driver::D3DDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
		break;
	case SOLID:
		Driver::D3DDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
		break;
	}

	mMaterial->Render(this, mGeo, camera);
}