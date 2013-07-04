#ifndef SUBMODEL_H
#define SUBMODEL_H

#include "RenderableObject.h"

class SubModel : public RenderableObject
{
public:
	SubModel(Object* _parent = NULL, Mesh* _mesh = NULL, Material* _material = NULL) 
		:RenderableObject(_parent, _mesh, _material)
	{

	}
};

#endif