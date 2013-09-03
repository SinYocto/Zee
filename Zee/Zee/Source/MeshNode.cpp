#include "MeshNode.h"
#include "DebugDrawer.h"

void MeshNode::Draw(Camera* camera)
{
	_Assert(NULL != camera);

	bool isSolid = true;

	if(mAttribute.displayMode == WIRE_FRAME)
		isSolid = false;

	mMesh->Draw(LocalToWorldMatrix(), camera, isSolid);

	if(mAttribute.drawBBox && mAABBox.isValid())
	{
		DebugDrawer::DrawAABBox(mAABBox, 0xffff0000, camera);
	}
}

void MeshNode::calCurrentAABBox()
{
	_Assert(mMesh && mMesh->GetGeometry())
	mMesh->GetGeometry()->CalcDynamicAABBox(mWorldPos, mWorldOrient, &mAABBox);
}

Mesh* MeshNode::GetMesh()
{
	return mMesh;
}
