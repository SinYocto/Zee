#ifndef RENDERABLEOBJECT_H
#define RENDERABLEOBJECT_H

#include "Object.h"
#include"Mesh.h"
#include"Shader.h"
#include"Material.h"

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
		mesh->Grab();
		material->Grab();
	}

	~RenderableObject()
	{
		mesh->Drop();
		material->Drop();
	}

	void Draw(Camera* camera);

protected:
	Mesh* mesh;
	Material* material; 

	DisplayMode displayMode;
};

#endif