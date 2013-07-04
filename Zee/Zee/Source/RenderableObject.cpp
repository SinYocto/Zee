#include"RenderableObject.h"
#include"Camera.h"
#include"DirectionalLight.h"
#include"PointLight.h"

void RenderableObject::Draw(Camera* camera)
{
	if(NULL == mesh || NULL == material)
	{
		return;		// ������RenderableObject����ʱ��ָ��mesh��material����, ��ʱ����draw�����κ���
	}

	_Assert(NULL != camera);

	if(displayMode == WireFrame)
		gD3DDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );

	material->Render(this, mesh, camera);

	if(displayMode == WireFrame)
		gD3DDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
}