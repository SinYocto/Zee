#include "BillboardNode.h"
#include "Engine.h"
#include "Camera.h"

BillboardNode::BillboardNode( const wchar_t* name, float width, float height, D3DCOLOR color ) :SceneNode(name)
{
	mType = SCENE_NODE_BILLBOARD;
	mBillboard = New Billboard(width, height, color);

	gEngine->GetResourceManager()->AddBillboard(mBillboard);
	mBillboard->Grab();
}

void BillboardNode::Draw(Camera* camera)
{
	mBillboard->Draw(mWorldPos, camera);
}

Billboard* BillboardNode::GetBillboard()
{
	return mBillboard;
}

void BillboardNode::updateAABBoxSelf()
{
	Camera* camera = gEngine->GetSceneManager()->GetMainCamera();		// TODO:这里直接使用了mainCamera

	Vector3 pos[4];
	pos[0] = mWorldPos - 0.5f * mBillboard->GetWidth() * camera->GetWorldRight().Normalized()
		- 0.5f * mBillboard->GetHeight() * camera->GetWorldUp().Normalized();

	pos[1] = mWorldPos - 0.5f * mBillboard->GetWidth() * camera->GetWorldRight().Normalized()
		+ 0.5f * mBillboard->GetHeight() * camera->GetWorldUp().Normalized();

	pos[2] = mWorldPos + 0.5f * mBillboard->GetWidth() * camera->GetWorldRight().Normalized()
		- 0.5f * mBillboard->GetHeight() * camera->GetWorldUp().Normalized();

	pos[3] = mWorldPos + 0.5f * mBillboard->GetWidth() * camera->GetWorldRight().Normalized()
		+ 0.5f * mBillboard->GetHeight() * camera->GetWorldUp().Normalized();

	mAABBox.mMin = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
	mAABBox.mMax = Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	for(int i = 0; i < 4; ++i)
	{
		if(pos[i].x > mAABBox.mMax.x)
			mAABBox.mMax.x = pos[i].x;
		if(pos[i].x < mAABBox.mMin.x)
			mAABBox.mMin.x = pos[i].x;

		if(pos[i].y > mAABBox.mMax.y)
			mAABBox.mMax.y = pos[i].y;
		if(pos[i].y < mAABBox.mMin.y)
			mAABBox.mMin.y = pos[i].y;

		if(pos[i].z > mAABBox.mMax.z)
			mAABBox.mMax.z = pos[i].z;
		if(pos[i].z < mAABBox.mMin.z)
			mAABBox.mMin.z = pos[i].z;
	}
}