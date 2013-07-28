#include"RenderableObject.h"
#include"Camera.h"
#include"DirectionalLight.h"
#include"PointLight.h"

void RenderableObject::Draw(Camera* camera)
{
	if(NULL == mGeo || NULL == mMaterial)
	{
		return;		// ������RenderableObject����ʱ��ָ��geo��material����, ��ʱ����draw�����κ���
	}

	_Assert(NULL != camera);

	if(mDisplayMode == WireFrame)
		gD3DDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );

	mMaterial->Render(this, mGeo, camera);

	if(mDisplayMode == WireFrame)
		gD3DDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
}