#include "PointLightNode.h"
#include "Billboard.h"
#include "Engine.h"
#include "Camera.h"

PointLightNode::PointLightNode(SceneNode* parent, PointLight* pointLight)
:SceneNode(pointLight->GetName(), parent)
,mPointLight(pointLight)
{
	mType = SCENE_NODE_POINT_LIGHT;
	mBillboard = new Billboard();		// TODO:û��Ҫÿ��lightһ��billboard

	mBillboard->SetTexture(L"./Assets/Textures/pointLight.jpg");
	mBillboard->SetColor(pointLight->GetFinalColor());

	gEngine->GetResourceManager()->AddBillboard(mBillboard);
	mBillboard->Grab();

	SetWorldPosition(pointLight->GetPosition());
}

PointLightNode::~PointLightNode()
{
	SAFE_DROP(mBillboard);
}

void PointLightNode::updateAABBox()
{
	Camera* camera = gEngine->GetSceneManager()->GetMainCamera();

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

void PointLightNode::OnTransformChanged()
{
	mPointLight->SetPosition(GetWorldPosition());
}

Billboard* PointLightNode::GetBillboard()
{
	return mBillboard;
}
