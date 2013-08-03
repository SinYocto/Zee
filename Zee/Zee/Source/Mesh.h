#ifndef MESH_H
#define MESH_H

#include "SceneNode.h"
#include "Geometry.h"
#include "Material.h"

class Mesh : public SceneNode
{
public:
	Mesh(const wchar_t* name, SceneNode* parent, Geometry* geo, Material* material) 
		:SceneNode(name, parent)
		,mGeo(geo)
		,mMaterial(material)
	{
		// TODO:���캯��������ʱ��geo��mtlΪnull,������Ϸ�������geo��material����
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

	void Draw(Camera* camera);

private:
	Geometry* mGeo;
	Material* mMaterial; 
};

#endif