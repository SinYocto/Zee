#ifndef MESH_NODE_H
#define MESH_NODE_H

#include "SceneNode.h"
#include "Mesh.h"

class MeshNode : public SceneNode
{
public:
	MeshNode(const wchar_t* name, SceneNode* parent, Geometry* geo, Material* material);
	MeshNode(const wchar_t* name, SceneNode* parent, Mesh* mesh);

	~MeshNode()
	{
		SAFE_DROP(mMesh);
	}
	
	virtual void Draw(Camera* camera);

	Mesh* GetMesh();

private:
	virtual void updateAABBoxSelf();

private:
	Mesh* mMesh;
};

#endif