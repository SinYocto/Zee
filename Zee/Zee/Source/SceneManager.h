#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include "SceneNode.h"

class Camera;

class SceneManager
{
public:
	SceneManager();

	void Init();
	void Destory();

	void FrameUpdate();

	SceneNode* GetRoot();
	Camera* GetMainCamera();
	Camera* GetExtraCamera();

	void CreateMainCamera(const Vector3 pos = Vector3(0, 0, -200), const Vector3 target = Vector3::Zero, 
		float fov = PI/2, float aspect = 1.0f, float nZ = 1.0f, float fZ = 1000.0f);
	void CreateExtraCamera(const Vector3 pos = Vector3(0, 0, -200), const Vector3 target = Vector3::Zero, 
		float fov = PI/2, float aspect = 1.0f, float nZ = 1.0f, float fZ = 1000.0f);

	void AddSceneNode(SceneNode* node, SceneNode* parent = NULL);
	void GetSceneNode(const wchar_t* name, SceneNode** sceneNode);

	SceneNode* RayIntersect(const Vector3& rayPos, const Vector3& rayDir, Vector3* hitPos, float* dist);

	void DrawAll();

private:
	SceneNode* root;
	Camera* mainCamera;
	Camera* extraCamera;

	DWORD curNodeID;
};

#endif