#ifndef RENDERABLEOBJECT_H
#define RENDERABLEOBJECT_H

#include "Object.h"
#include"Mesh.h"
#include"Shader.h"
#include"Material.h"

// #��Դ����
// RenderableObject������һ��ָ��mesh�����һ��ָ��material�����ָ��, ����ʱ��Ҫ���������������Grab, ����ʱDrop

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