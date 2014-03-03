#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include "SceneNode.h"
#include "Shader.h"

class Camera;
class MeshNode;
class BillboardNode;
class DirectionalLightNode;
class PointLightNode;
class Terrain;

class SceneManager
{
public:
	SceneManager();

	void OnLostDevice();
	void OnResetDevice();

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
	SceneNode* AddPrimitiveCube();

	void AddTerrain(Terrain* terrain);

	SceneNode* RayIntersect(const Vector3& rayPos, const Vector3& rayDir, Vector3* hitPos, float* dist);

	void DrawAll();

	void DrawAllUseRenderer();

	bool NeedDirLightShadow();

private:
	void collectSceneEntities();
	void collectSceneNode(SceneNode* sceneNode);

	void collectLightViewSceneEntities();
	void collectLightViewSceneNode(SceneNode* sceneNode);

	void drawShadowMapPass();
	void drawShadowTexPass();

private:
	SceneNode* root;
	Camera* mainCamera;
	Camera* extraCamera;

	typedef std::list<MeshNode*> MeshNodeList;
	typedef std::map<ShadingMethod, MeshNodeList> ShadingMethodMeshNodeListMap;

	MeshNodeList mWireFrameMeshNodeList;
	ShadingMethodMeshNodeListMap mMeshNodeLists;		// ½ö°üº¬MeshNodeºÍModelNode
	std::list<AABBox> mAABBoxes;
	std::list<BillboardNode*> mBillboardNodeList;

	std::list<DirectionalLightNode*> mDirLightNodes;
	std::list<PointLightNode*> mPointLihgtNodes; 

	std::list<MeshNode*> mShadowMapMeshNodeList;
	DirectionalLightNode* mShadowMapDirLightNode;

	Terrain* mTerrain;
};

#endif