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
		gD3DDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
		break;
	case SOLID:
		gD3DDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
		break;
	}

	mMaterial->Render(this, mGeo, camera);
}