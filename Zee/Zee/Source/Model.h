#ifndef MODEL_H
#define MODEL_H

#include "Mesh.h"

enum ModelFileFormat
{
	MODEL_OBJ
};

class Camera;
class AABBox;

class Model : public IReferenceCounted
{
public:
	Model(Geometry* geo, Material* material)
	{
		Mesh* mesh = new Mesh(geo, material);
		AddSubMesh(mesh);
	}

	~Model()
	{
		for(std::list<Mesh*>::iterator iter = mSubMeshes.begin(); iter != mSubMeshes.end(); ++iter)
		{
			SAFE_DROP(*iter);
		}
	}

	void AddSubMesh(Mesh* mesh);
	void LoadModelDataFromFile(wchar_t* filename, ModelFileFormat format);

	void Draw(const D3DXMATRIX& matWorld, Camera* camera, bool isSolid);

	void CalcDynamicAABBox(const Vector3& pos, const Quaternion& orient, AABBox* box);

private:
	std::list<Mesh*> mSubMeshes;
};


#endif