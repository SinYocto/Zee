#ifndef MESH_H
#define MESH_H

#include "Geometry.h"
#include "Material.h"

class Mesh : public IReferenceCounted
{
public:
	Mesh(const wchar_t* name, Geometry* geo, Material* material);
	Mesh(const wchar_t* name, Mesh* mesh);

	~Mesh()
	{
		SAFE_DROP(mGeo);
		SAFE_DROP(mMaterial);
	}

	Geometry* GetGeometry();
	Material* GetMaterial();
	void SetMaterial(Material* mtl);
	void Draw(const D3DXMATRIX& matWorld, Camera* camera, bool isSolid);
	void DrawUseMtl(const D3DXMATRIX& matWorld, Camera* camera, Material* mtl);

	wchar_t* GetName();

private:
	wchar_t mName[MAX_STR_LEN];

	Geometry* mGeo;
	Material* mMaterial; 
};

#endif