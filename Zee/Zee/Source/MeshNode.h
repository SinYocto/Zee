#ifndef MESH_NODE_H
#define MESH_NODE_H

#include "SceneNode.h"
#include "Mesh.h"

class MeshNode : public SceneNode
{
public:
	MeshNode(const wchar_t* name, SceneNode* parent, Geometry* geo, Material* material)
		:SceneNode(name, parent)
	{
		mType = SCENE_NODE_BILLBOARD;
		mMesh = new Mesh(geo, material);
	}

	~MeshNode()
	{
		SAFE_DROP(mMesh);
	}
	
	virtual void Draw(Camera* camera);

	Mesh* GetMesh();

private:
	virtual void calCurrentAABBox();

private:
	Mesh* mMesh;
};

#endif