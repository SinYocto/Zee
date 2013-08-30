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
		mType = SCENE_NODE_MESH;

		// TODO:���캯��������ʱ��geo��mtlΪnull,������Ϸ�������geo��material����
		if(NULL != mGeo)
		{
			mGeo->Grab();
			mGeo->CalcDynamicAABBox(mWorldPos, mWorldOrient, &mAABBox);
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

	virtual void Draw(Camera* camera);
	void SetMaterial(Material* mtl);

	Geometry* GetGeometry();

private:
	virtual void calCurrentAABBox();

private:
	Geometry* mGeo;
	Material* mMaterial; 
};

#endif