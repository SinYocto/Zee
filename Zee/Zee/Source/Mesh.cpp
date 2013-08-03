#include "Mesh.h"

void Mesh::Draw(Camera* camera)
{
	if(NULL == mGeo || NULL == mMaterial)
	{
		return;		// 允许构造Mesh对象时不指定geo和material对象, 此时调用draw不做任何事
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