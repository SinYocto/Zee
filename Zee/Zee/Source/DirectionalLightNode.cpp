#include "DirectionalLightNode.h"
#include "Billboard.h"
#include "Engine.h"
#include "Camera.h"

DirectionalLightNode::DirectionalLightNode(SceneNode* parent, DirectionalLight* dirLight)
:SceneNode(dirLight->GetName(), parent)
,mDirLight(dirLight)
{
	mType = SCENE_NODE_DIR_LIGHT;
	mBillboard = New Billboard();

	mBillboard->SetTexture(L"./Assets/Textures/Editor/DirLight.jpg");
	mBillboard->SetColor(dirLight->GetFinalColor());

	gEngine->GetResourceManager()->AddBillboard(mBillboard);
	mBillboard->Grab();

	Quaternion orient = Quaternion::VectorRotation(Vector3(0, 0, 1.0f), dirLight->GetDirection());
	SetWorldOrientation(orient);
}

DirectionalLightNode::~DirectionalLightNode()
{
	SAFE_DROP(mBillboard);
}

void DirectionalLightNode::updateAABBoxSelf()
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

void DirectionalLightNode::OnTransformChanged()
{
	Vector3 dir = Vector3(0, 0, 1.0f) * GetWorldOrient();
	mDirLight->SetDirection(dir);

	SceneNode::OnTransformChanged();
}

Billboard* DirectionalLightNode::GetBillboard()
{
	return mBillboard;
}

DirectionalLight* DirectionalLightNode::GetDirLight()
{
	return mDirLight;
}

void DirectionalLightNode::SetLightDirection(const Vector3& dir)
{
	mDirLight->SetDirection(dir);
}

void DirectionalLightNode::SetLightIntensity(float intensity)
{
	mDirLight->SetIntensity(intensity);
	mBillboard->SetColor(mDirLight->GetFinalColor());
}

void DirectionalLightNode::SetLightColor(D3DXCOLOR color)
{
	mDirLight->SetColor(color);
	mBillboard->SetColor(mDirLight->GetFinalColor());
}

bool DirectionalLightNode::IsLightEnabled()
{
	return mDirLight->IsEnabled();
}
