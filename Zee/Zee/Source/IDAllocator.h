#ifndef ID_ALLOCATOR_H
#define ID_ALLOCATOR_H

#include "Common.h"

class Geometry;
class Material;
class Mesh;
class Model;
class SceneNode;

class IDAllocator
{
public:
	enum 
	{
		ID_ANY = 0xffffffff,
	};

public:
	IDAllocator();

	void AllocateGeometryID(Geometry* geo);
	void AllocateMaterialID(Material* mtl);
	void AllocateMeshID(Mesh* mesh);
	void AllocateModelID(Model* model);
	void AllocateSceneNodeID(SceneNode* sceneNode);

private:
	DWORD mCurGeometryID;
	DWORD mCurMaterialID;
	DWORD mCurMeshID;
	DWORD mCurModelID;
	DWORD mCurSceneNodeID;
};

#endif