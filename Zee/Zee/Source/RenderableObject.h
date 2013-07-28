#ifndef RENDERABLEOBJECT_H
#define RENDERABLEOBJECT_H

#include "Object.h"
#include"Geometry.h"
#include"Shader.h"
#include"Material.h"

// #资源管理
// RenderableObject持有了一个指向geo对象和一个指向material对象的指针, 构造时需要对这两个对象进行Grab, 析构时Drop

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