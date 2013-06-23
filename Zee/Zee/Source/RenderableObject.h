#ifndef RENDERABLEOBJECT_H
#define RENDERABLEOBJECT_H

#include "Object.h"
#include"Mesh.h"
#include"Shader.h"
#include"Material.h"

// #资源管理
// RenderableObject持有了一个指向mesh对象和一个指向material对象的指针, 构造时需要对这两个对象进行Grab, 析构时Drop

enum DisplayMode { Textured, WireFrame };

class Camera;

class RenderableObject : public Object
{
public:
	RenderableObject(Object* _parent = NULL, Mesh* _mesh = NULL, Material* _material = NULL)
		:Object(_parent)
		,mesh(_mesh)
		,material(_material)
		,displayMode(Textured)
	{
		if(NULL != mesh)
		{
			mesh->Grab();
		}

		if(NULL != material)
		{
			material->Grab();
		}
	}

	~RenderableObject()
	{
		SAFE_DROP(mesh);
		SAFE_DROP(material);
	}

	void Draw(Camera* camera);

protected:
	Mesh* mesh;
	Material* material; 

	DisplayMode displayMode;
};

#endif