#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include "SceneNode.h"

class Camera;

class SceneManager
{
public:
	static void Init();
	static void CreateMainCamera(const Vector3 pos = Vector3(0, 0, -200), const Vector3 target = Vector3::Zero, 
		float fov = PI/2, float aspect = 1.0f, float nZ = 1.0f, float fZ = 1000.0f);

	static void Destory();
	static void AddSceneNode(SceneNode* node, SceneNode* parent);

	static void FrameUpdate();

	static void DrawAll();

public:
	static SceneNode* root;
	static Camera* mainCamera;

private:
	static DWORD curID;
};

#endif