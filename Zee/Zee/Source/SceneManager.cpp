#include "SceneManager.h"
#include "Camera.h"
#include "MeshNode.h"
#include "BillboardNode.h"
#include "DirectionalLightNode.h"
#include "PointLightNode.h"
#include "Engine.h"
#include "Renderer.h"
#include "DebugDrawer.h"
#include "ModelNode.h"
#include "ShadowMapRenderer.h"
#include "Terrain.h"

SceneManager::SceneManager()
:root(NULL)
,mainCamera(NULL)
,extraCamera(NULL)
,mShadowMapDirLightNode(NULL)
,mTerrain(NULL)
{

}

void SceneManager::OnLostDevice()
{
	if(mTerrain)
		mTerrain->OnLostDevice();
}

void SceneManager::OnResetDevice()
{
	if(mTerrain)
		mTerrain->OnResetDevice();
}

void SceneManager::Init()
{
	root = New SceneNode(L"root");

	gEngine->GetIDAllocator()->AllocateSceneNodeID(root);
}

void SceneManager::AddSceneNode(SceneNode* node, SceneNode* parent /*= root*/)
{
	_Assert(NULL != node);

	if(parent == NULL)
		parent = root;

	parent->AddChild(node);
	gEngine->GetIDAllocator()->AllocateSceneNodeID(node);
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
	SAFE_DELETE(mTerrain);
}

void SceneManager::CreateMainCamera( const Vector3 pos /*= Vector3(0, 0, -200)*/, const Vector3 target /*= Vector3::Zero*/, 
								 float fov /*= PI/2*/, float aspect /*= 1.0f*/, float nZ /*= 1.0f*/, float fZ /*= 1000.0f*/ )
{
	mainCamera = New Camera(pos, target, fov, aspect, nZ, fZ);
}

void SceneManager::CreateExtraCamera( const Vector3 pos /*= Vector3(0, 0, -200)*/, const Vector3 target /*= Vector3::Zero*/, 
									float fov /*= PI/2*/, float aspect /*= 1.0f*/, float nZ /*= 1.0f*/, float fZ /*= 1000.0f*/ )
{
	_Assert(extraCamera == NULL);
	extraCamera = New Camera(pos, target, fov, aspect, nZ, fZ);
}

void SceneManager::DrawAll()
{
	_Assert(NULL != mainCamera);
	root->DrawAll(mainCamera);
}

void SceneManager::DrawAllUseRenderer()
{
	if(mShadowMapDirLightNode != NULL)
	{
		drawShadowMapPass();
		drawShadowTexPass();
	}
	// terrain
	if(mTerrain)
		mTerrain->Draw(mainCamera, true);

	// AABBoxes
	for(std::list<AABBox>::iterator iter = mAABBoxes.begin(); iter != mAABBoxes.end(); ++iter)
	{
		AABBox& box = *iter;
		DebugDrawer::DrawAABBox(box, 0xffff0000, mainCamera);
	}

	// wireframe
	Renderer::Begin(WireFrame);
	for(MeshNodeList::iterator iter = mWireFrameMeshNodeList.begin(); iter != mWireFrameMeshNodeList.end(); ++iter)
	{
		MeshNode* meshNode = *iter;
		Mesh* mesh = meshNode->GetMesh();

		MaterialManager* materialMgr = gEngine->GetMaterialManager();

		Renderer::DrawMesh(meshNode->LocalToWorldMatrix(), mesh->GetGeometry(), materialMgr->GetDefaultFlatMtl(), mainCamera);
	}
	Renderer::End(WireFrame);

	// 以ShadingMethod为分类, 统一设置RenderState然后绘制
	for(ShadingMethodMeshNodeListMap::iterator mapIter = mMeshNodeLists.begin(); mapIter != mMeshNodeLists.end(); ++mapIter)
	{
		ShadingMethod method = mapIter->first;
		MeshNodeList& meshNodeList = mapIter->second;

		Renderer::Begin(method);
		for(MeshNodeList::iterator listIter = meshNodeList.begin(); listIter != meshNodeList.end(); ++listIter)
		{
			MeshNode* meshNode = *listIter;
			Mesh* mesh = meshNode->GetMesh();

			Renderer::DrawMesh(meshNode->LocalToWorldMatrix(), mesh->GetGeometry(), mesh->GetMaterial(), mainCamera);
		}
		Renderer::End(method);
	}

	// light billboard
	if(!mDirLightNodes.empty() || !mPointLihgtNodes.empty())
	{
		Renderer::Begin(BillboardMethod);

		for(std::list<DirectionalLightNode*>::iterator iter = mDirLightNodes.begin(); iter != mDirLightNodes.end(); ++iter)
		{
			DirectionalLightNode* dirLightNode = *iter;

			Renderer::DrawBillboard(dirLightNode->GetWorldPosition(), dirLightNode->GetBillboard(), mainCamera);
		}

		for(std::list<PointLightNode*>::iterator iter = mPointLihgtNodes.begin(); iter != mPointLihgtNodes.end(); ++iter)
		{
			PointLightNode* pointLightNode = *iter;

			Renderer::DrawBillboard(pointLightNode->GetWorldPosition(), pointLightNode->GetBillboard(), mainCamera);
		}

		Renderer::End(BillboardMethod);
	}

	// billboard
	if(!mBillboardNodeList.empty())
	{
		Renderer::Begin(BillboardMethod);
		for(std::list<BillboardNode*>::iterator iter = mBillboardNodeList.begin(); iter != mBillboardNodeList.end(); ++iter)
		{
			BillboardNode* billBoradNode = *iter;

			Renderer::DrawBillboard(billBoradNode->GetWorldPosition(), billBoradNode->GetBillboard(), mainCamera);
		}
		Renderer::End(BillboardMethod);
	}
}

void SceneManager::FrameUpdate()
{
	_Assert(NULL != mainCamera);
	mainCamera->FrameUpdate();

	if(extraCamera)
		extraCamera->FrameUpdate();

	if(mTerrain)
		mTerrain->FrameUpdate(mainCamera);

	collectSceneEntities();
	collectLightViewSceneEntities();
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

void SceneManager::collectSceneEntities()
{
	mMeshNodeLists.clear();
	mWireFrameMeshNodeList.clear();
	mBillboardNodeList.clear();
	mAABBoxes.clear();

	mDirLightNodes.clear();
	mPointLihgtNodes.clear();

	collectSceneNode(root);
}

void SceneManager::collectSceneNode(SceneNode* sceneNode)
{
	if(!mainCamera->IsVisible(sceneNode->GetAABBox()))
		return;

	if(sceneNode->GetDrawBBoxFlag())
	{
		AABBox box = sceneNode->GetAABBox();

		if(box.isValid())
			mAABBoxes.push_back(box);
	}

	if(sceneNode->GetNodeType() == SceneNode::SCENE_NODE_MESH)
	{
		MeshNode* meshNode = static_cast<MeshNode*>(sceneNode);

		Mesh* mesh = meshNode->GetMesh();
		_Assert(NULL != mesh);

		if(sceneNode->GetDisplayMode() == SceneNode::SOLID || sceneNode->GetDisplayMode() == SceneNode::SOLID_WIRE_FRAME)
		{
			Material* meshMaterial = meshNode->GetMesh()->GetMaterial();
			_Assert(NULL != meshMaterial);

			ShadingMethod method = meshMaterial->GetShadingMethod();
 
			ShadingMethodMeshNodeListMap::iterator iter = mMeshNodeLists.find(method);
			if(iter != mMeshNodeLists.end())
			{
				iter->second.push_back(meshNode);
			}
			else
			{
				MeshNodeList meshList;
				meshList.push_back(meshNode);
				mMeshNodeLists[method] = meshList;
			}
		}

		if(sceneNode->GetDisplayMode() == SceneNode::WIRE_FRAME || sceneNode->GetDisplayMode() == SceneNode::SOLID_WIRE_FRAME)
		{
			mWireFrameMeshNodeList.push_back(meshNode);
		}
	}
	else if(sceneNode->GetNodeType() == SceneNode::SCENE_NODE_BILLBOARD)
	{
		BillboardNode* billboardNode = static_cast<BillboardNode*>(sceneNode);
		mBillboardNodeList.push_back(billboardNode);
	}
	else if(sceneNode->GetNodeType() == SceneNode::SCENE_NODE_DIR_LIGHT)
	{
		DirectionalLightNode* dirLightNode = static_cast<DirectionalLightNode*>(sceneNode);
		mDirLightNodes.push_back(dirLightNode);
	}
	else if(sceneNode->GetNodeType() == SceneNode::SCENE_NODE_POINT_LIGHT)
	{
		PointLightNode* pointLightNode = static_cast<PointLightNode*>(sceneNode);
		mPointLihgtNodes.push_back(pointLightNode);
	}

	std::list<Object*>* children = NULL;
	sceneNode->GetChildrenV2(children);
	for(std::list<Object*>::iterator iter = (*children).begin(); iter != (*children).end(); ++iter)
	{
		SceneNode* node = static_cast<SceneNode*>(*iter);
		collectSceneNode(node);
	}	
}

SceneNode* SceneManager::AddPrimitiveCube()
{	
	ModelNode* cube = New ModelNode(L"cube", NULL, new Model(L"cube", L"Assets/Models/defaultCube.model"));
	AddSceneNode(cube);
	cube->OnTransformChanged();

	return cube;
}

void SceneManager::drawShadowMapPass()
{
	ShadowMapRenderer::BeginShadowMapPass();
	ShadowMapRenderer::SetupVirtualLightCamera(mShadowMapDirLightNode);
	for(MeshNodeList::iterator iter = mShadowMapMeshNodeList.begin(); iter != mShadowMapMeshNodeList.end(); ++iter)
	{
		MeshNode* meshNode = *iter;
		Mesh* mesh = meshNode->GetMesh();

		ShadowMapRenderer::DrawMeshShadowMapPass(meshNode->LocalToWorldMatrix(), mesh->GetGeometry());
	}

	ShadowMapRenderer::DrawTerrainShadowMapPass(mTerrain);

	ShadowMapRenderer::EndShadowMapPass();

	ShadowMapRenderer::ShadowMapGaussianBlur();
}

void SceneManager::drawShadowTexPass()
{
	ShadowMapRenderer::BeginShadowTexPass();

	for(ShadingMethodMeshNodeListMap::iterator mapIter = mMeshNodeLists.begin(); mapIter != mMeshNodeLists.end(); ++mapIter)
	{
		ShadingMethod method = mapIter->first;
		MeshNodeList& meshNodeList = mapIter->second;

		for(MeshNodeList::iterator listIter = meshNodeList.begin(); listIter != meshNodeList.end(); ++listIter)
		{
			MeshNode* meshNode = *listIter;
			Mesh* mesh = meshNode->GetMesh();

			ShadowMapRenderer::DrawMeshShadowTexPass(meshNode->LocalToWorldMatrix(), mesh->GetGeometry(), mainCamera);
		}
	}

	ShadowMapRenderer::DrawTerrainShadowTexPass(mTerrain, mainCamera);

	ShadowMapRenderer::EndShadowTexPass();
}


void SceneManager::collectLightViewSceneEntities()
{
	mShadowMapMeshNodeList.clear();
	mShadowMapDirLightNode = NULL;
	collectLightViewSceneNode(root);
}

void SceneManager::collectLightViewSceneNode( SceneNode* sceneNode )
{
	if(sceneNode->GetNodeType() == SceneNode::SCENE_NODE_MESH)
	{
		if(sceneNode->GetAABBox().IntersectedWith(ShadowMapRenderer::GetVirtualCameraBound()))
		{
			MeshNode* meshNode = static_cast<MeshNode*>(sceneNode);
			mShadowMapMeshNodeList.push_back(meshNode);
		}
	}
	else if(sceneNode->GetNodeType() == SceneNode::SCENE_NODE_DIR_LIGHT)
	{
		DirectionalLightNode* dirLightNode = static_cast<DirectionalLightNode*>(sceneNode);

		if(mShadowMapDirLightNode == NULL && dirLightNode->IsLightEnabled())
		{
			mShadowMapDirLightNode = dirLightNode;
		}
	}

	std::list<Object*>* children = NULL;
	sceneNode->GetChildrenV2(children);
	for(std::list<Object*>::iterator iter = (*children).begin(); iter != (*children).end(); ++iter)
	{
		SceneNode* node = static_cast<SceneNode*>(*iter);
		collectLightViewSceneNode(node);
	}	
}

bool SceneManager::NeedDirLightShadow()
{
	return (mShadowMapDirLightNode != NULL);
}

void SceneManager::AddTerrain(Terrain* terrain)
{
	_Assert(mTerrain == NULL);
	mTerrain = terrain;
}
