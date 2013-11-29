#include "IDAllocator.h"
#include "Geometry.h"
#include "Material.h"
#include "SceneNode.h"

IDAllocator::IDAllocator()
:mCurGeometryID(0)
,mCurMaterialID(0)
,mCurMeshID(0)
,mCurModelID(0)
,mCurSceneNodeID(0)
{

}

void IDAllocator::AllocateGeometryID(Geometry* geo)
{
	_Assert(NULL != geo);
	geo->SetID(mCurGeometryID++);
}

void IDAllocator::AllocateMaterialID(Material* mtl)
{
	_Assert(NULL != mtl);
	mtl->SetID(mCurMaterialID++);
}

void IDAllocator::AllocateSceneNodeID(SceneNode* sceneNode)
{
	_Assert(NULL != sceneNode);
	sceneNode->SetID(mCurSceneNodeID++);
}
