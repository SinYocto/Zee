#ifndef RENDERABLEOBJECT_H
#define RENDERABLEOBJECT_H

#include "Object.h"
#include"Geometry.h"
#include"Shader.h"
#include"Material.h"

// #��Դ����
// RenderableObject������һ��ָ��geo�����һ��ָ��material�����ָ��, ����ʱ��Ҫ���������������Grab, ����ʱDrop

enum DisplayMode { Textured, WireFrame };

class Camera;

class RenderableObject : public Object
{
public:
	RenderableObject(Object* parent, Geometry* geo, Material* material)
		:Object(parent)
		,mGeo(geo)
		,mMaterial(material)
		,mDisplayMode(Textured)
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

	~RenderableObject()
	{
		SAFE_DROP(mGeo);
		SAFE_DROP(mMaterial);
	}

	void Draw(Camera* camera);

protected:
	Geometry* mGeo;
	Material* mMaterial; 

	DisplayMode mDisplayMode;
};

#endif