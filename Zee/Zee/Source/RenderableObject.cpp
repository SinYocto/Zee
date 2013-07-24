#include"RenderableObject.h"
#include"Camera.h"
#include"DirectionalLight.h"
#include"PointLight.h"

void RenderableObject::Draw(Camera* camera)
{
	if(NULL == geo || NULL == material)
	{
		return;		// 允许构造RenderableObject对象时不指定geo和material对象, 此时调用draw不做任何事
	}

	_Assert(NULL != camera);

	if(displayMode == WireFrame)
		gD3DDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );

	material->Render(this, geo, camera);

	if(displayMode == WireFrame)
		gD3DDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
}