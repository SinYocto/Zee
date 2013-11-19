#include "SceneManager.h"
#include "Camera.h"

SceneManager::SceneManager()
:root(NULL)
,mainCamera(NULL)
,extraCamera(NULL)
,curNodeID(0)
{

}

void SceneManager::Init()
{
	root = new SceneNode(L"root");
	root->SetID(curNodeID++); 
}

void SceneManager::AddSceneNode(SceneNode* node, SceneNode* parent /*= root*/)
{
	_Assert(NULL != node);

	if(parent == NULL)
		parent = root;

	parent->AddChild(node);
	node->SetID(curNodeID++);			// QUESTION:ID一直加不会溢出吧
}

void SceneManager::GetSceneNode(const wchar_t* name, SceneNode** sceneNode)
{
	// TODO
	// ...
}


void SceneManager::Destory()
{
	SAFE_DELETE(root);
	SAFE_DELETE(mainCamera);
	SAFE_DELETE(extraCamera);
}

void SceneManager::CreateMainCamera( const Vector3 pos /*= Vector3(0, 0, -200)*/, const Vector3 target /*= Vector3::Zero*/, 
								 float fov /*= PI/2*/, float aspect /*= 1.0f*/, float nZ /*= 1.0f*/, float fZ /*= 1000.0f*/ )
{
	mainCamera = new Camera(pos, target, fov, aspect, nZ, fZ);
}

void SceneManager::CreateExtraCamera( const Vector3 pos /*= Vector3(0, 0, -200)*/, const Vector3 target /*= Vector3::Zero*/, 
									float fov /*= PI/2*/, float aspect /*= 1.0f*/, float nZ /*= 1.0f*/, float fZ /*= 1000.0f*/ )
{
	_Assert(extraCamera == NULL);
	extraCamera = new Camera(pos, target, fov, aspect, nZ, fZ);
}

void SceneManager::DrawAll()
{
	_Assert(NULL != mainCamera);
	root->DrawAll(mainCamera);
}

void SceneManager::FrameUpdate()
{
	_Assert(NULL != mainCamera);
	mainCamera->FrameUpdate();

	if(extraCamera)
		extraCamera->FrameUpdate();

	root->FrameUpdate();
}

SceneNode* SceneManager::RayIntersect(const Vector3& rayPos, const Vector3& rayDir, Vector3* hitPos, float* dist)
{
	return root->RayIntersect(rayPos, rayDir, hitPos, dist);
}

SceneNode* SceneManager::GetRoot()
{
	return root;
}

Camera* SceneManager::GetMainCamera()
{
	return mainCamera;
}

Camera* SceneManager::GetExtraCamera()
{
	return extraCamera;
}
