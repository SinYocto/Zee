#ifndef MESH_H
#define MESH_H

#include "RenderableObject.h"

class Mesh : public RenderableObject
{
public:
	Mesh(Object* _parent, Geometry* _geo, Material* _material) 
		:RenderableObject(_parent, _geo, _material)
	{

	}
};

#endif