#ifndef MESH_H
#define MESH_H

#include "Geometry.h"
#include "Material.h"

class Mesh : public IReferenceCounted
{
public:
	Mesh(Geometry* geo, Material* material)
		:mGeo(geo)
		,mMaterial(material)
	{
		if(NULL != mGeo)
		{
			mGeo->Grab();
		}

		if(NULL != mMaterial)
		{
			mMaterial->Grab();
		}
	}

	~Mesh()
	{
		SAFE_DROP(mGeo);
		SAFE_DROP(mMaterial);
	}

	Geometry* GetGeometry();
	void SetMaterial(Material* mtl);
	void Draw(const D3DXMATRIX& matWorld, Camera* camera, bool isSolid);

private:
	Geometry* mGeo;
	Material* mMaterial; 
};

#endif