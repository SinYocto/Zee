#include"RenderableObject.h"
#include"Camera.h"
#include"DirectionalLight.h"
#include"PointLight.h"

void RenderableObject::Draw(Camera* camera)
{
	if(NULL == mGeo || NULL == mMaterial)
	{
		return;		// 允许构造RenderableObject对象时不指定geo和material对象, 此时调用draw不做任何事
	}

	_Assert(NULL != camera);

	if(mDisplayMode == WireFrame)
		gD3DDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );

	mMaterial->Render(this, mGeo, camera);

	if(mDisplayMode == WireFrame)
		gD3DDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
}