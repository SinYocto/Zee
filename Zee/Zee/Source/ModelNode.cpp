#include "ModelNode.h"
#include "DebugDrawer.h"

void ModelNode::calcCurrentAABBox()
{
	mModel->CalcDynamicAABBox(mWorldPos, mWorldOrient, &mAABBox);
}

void ModelNode::Draw(Camera* camera)
{
	_Assert(NULL != camera);

	bool isSolid = true;

	if(mAttribute.displayMode == WIRE_FRAME)
		isSolid = false;

	mModel->Draw(LocalToWorldMatrix(), camera, isSolid);

	if(mAttribute.drawBBox && mAABBox.isValid())
	{
		DebugDrawer::DrawAABBox(mAABBox, 0xffff0000, camera);
	}
}
