#include "MeshNode.h"
#include "DebugDrawer.h"

MeshNode::MeshNode(const wchar_t* name, SceneNode* parent, Geometry* geo, Material* material) 
:SceneNode(name, parent)
{
	mType = SCENE_NODE_MESH;
	mMesh = new Mesh(name, geo, material);
}

MeshNode::MeshNode(const wchar_t* name, SceneNode* parent, Mesh* mesh)
:SceneNode(name, parent)
{
	_Assert(NULL != mesh);
	mType = SCENE_NODE_MESH;
	mMesh = mesh;
	mMesh->Grab();
}

void MeshNode::Draw(Camera* camera)
{
	_Assert(NULL != camera);

	bool isSolid = true;

	if(mAttribute.displayMode == WIRE_FRAME)
		isSolid = false;

	mMesh->Draw(LocalToWorldMatrix(), camera, isSolid);

	if(mAttribute.drawBBox && mAABBox.isValid())
	{
		DebugDrawer::DrawAABBox(mAABBox, 0xffff0000, camera);
	}
}

void MeshNode::updateAABBox()
{
	_Assert(mMesh && mMesh->GetGeometry())
	mMesh->GetGeometry()->CalcDynamicAABBox(mWorldPos, mWorldOrient, &mAABBox);
}

Mesh* MeshNode::GetMesh()
{
	return mMesh;
}
