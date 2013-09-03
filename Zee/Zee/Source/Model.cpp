#include "Model.h"
#include "Camera.h"
#include "DebugDrawer.h"

void Model::LoadModelDataFromFile(wchar_t* filename, ModelFileFormat format)
{

}

void Model::AddSubMesh(Mesh* mesh)
{
	_Assert(NULL != mesh);

	if(std::find(mSubMeshes.begin(), mSubMeshes.end(), mesh) != mSubMeshes.end())
	{
		return;
	}
	else
	{
		mSubMeshes.push_back(mesh);
	}
}

void Model::Draw(const D3DXMATRIX& matWorld, Camera* camera, bool isSolid)
{
	for(std::list<Mesh*>::iterator iter = mSubMeshes.begin(); iter != mSubMeshes.end(); ++iter)
	{
		(*iter)->Draw(matWorld, camera, isSolid);
	}
}

void Model::CalcDynamicAABBox(const Vector3& pos, const Quaternion& orient, AABBox* box)
{
	_Assert(NULL != box);
	AABBox resultBox = AABBox::Invalid;

	for(std::list<Mesh*>::iterator iter = mSubMeshes.begin(); iter != mSubMeshes.end(); ++iter)
	{
		Mesh* mesh = *iter;
		AABBox subMeshBBox = AABBox::Invalid;

		mesh->GetGeometry()->CalcDynamicAABBox(pos, orient, &subMeshBBox);
		resultBox = AABBox::CombineBBox(resultBox, subMeshBBox);
	}

	*box = resultBox;
}
