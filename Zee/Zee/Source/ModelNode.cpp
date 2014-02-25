#include "ModelNode.h"
#include "MeshNode.h"
#include "DebugDrawer.h"
#include "Engine.h"

ModelNode::ModelNode(const wchar_t* name, SceneNode* parent, Model* model) 
:SceneNode(name, parent)
{
	mType = SCENE_NODE_MODEL;
	mModel = model;
	mModel->Grab();

	std::list<Mesh*> meshList = model->GetSubMeshList();

	for(std::list<Mesh*>::iterator iter = meshList.begin(); iter != meshList.end(); ++iter)
	{
		Mesh* mesh = *iter;
		MeshNode* meshNode = New MeshNode(L"mesh", this, mesh);
	}
}

ModelNode::ModelNode(const wchar_t* name, SceneNode* parent, Geometry* geo, Material* material) 
:SceneNode(name, parent)
{
	mType = SCENE_NODE_MODEL;
	mModel = New Model(name, geo, material);
	mModel->Grab();

	MeshNode* meshNode = New MeshNode(L"mesh", this, geo, material);
}

//void ModelNode::updateAABBox()
//{
//	mModel->CalcDynamicAABBox(mWorldPos, mWorldOrient, &mAABBox);
//}

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

Model* ModelNode::GetModel()
{
	return mModel;
}
