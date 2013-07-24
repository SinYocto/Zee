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
	RenderableObject(Object* _parent, Geometry* _geo, Material* _material)
		:Object(_parent)
		,geo(_geo)
		,material(_material)
		,displayMode(Textured)
	{
		if(NULL != geo)
		{
			geo->Grab();
		}

		if(NULL != material)
		{
			material->Grab();
		}
	}

	~RenderableObject()
	{
		SAFE_DROP(geo);
		SAFE_DROP(material);
	}

	void Draw(Camera* camera);

protected:
	Geometry* geo;
	Material* material; 

	DisplayMode displayMode;
};

#endif