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
	Model(const wchar_t* name, Geometry* geo, Material* material);

	~Model();

	void AddSubMesh(Mesh* mesh);
	void LoadModelDataFromFile(wchar_t* filename, ModelFileFormat format);

	void Draw(const D3DXMATRIX& matWorld, Camera* camera, bool isSolid);

	void CalcDynamicAABBox(const D3DXMATRIX& matWorld, AABBox* box);

	std::list<Mesh*> GetSubMeshList();

	wchar_t* GetName();

private:
	wchar_t mName[MAX_STR_LEN];
	std::list<Mesh*> mSubMeshes;
};


#endif