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