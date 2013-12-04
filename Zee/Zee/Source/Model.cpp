#include "Model.h"
#include "Camera.h"
#include "DebugDrawer.h"

Model::Model(const wchar_t* name, Geometry* geo, Material* material)
{
	YString::Copy(mName, _countof(mName), name);

	Mesh* mesh = new Mesh(name, geo, material);
	AddSubMesh(mesh);
	SAFE_DROP(mesh);
}

Model::~Model()
{
	for(std::list<Mesh*>::iterator iter = mSubMeshes.begin(); iter != mSubMeshes.end(); ++iter)
	{
		SAFE_DROP(*iter);
	}
}

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
		mesh->Grab();
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

void Model::CalcDynamicAABBox(const D3DXMATRIX& matWorld, AABBox* box)
{
	_Assert(NULL != box);
	AABBox resultBox = AABBox::Invalid;

	for(std::list<Mesh*>::iterator iter = mSubMeshes.begin(); iter != mSubMeshes.end(); ++iter)
	{
		Mesh* mesh = *iter;
		AABBox subMeshBBox = AABBox::Invalid;

		mesh->GetGeometry()->CalcDynamicAABBox(matWorld, &subMeshBBox);
		resultBox = AABBox::CombineBBox(resultBox, subMeshBBox);
	}

	*box = resultBox;
}

std::list<Mesh*> Model::GetSubMeshList()
{
	return mSubMeshes;
}

wchar_t* Model::GetName()
{
	return mName;
}
