#ifndef SUBMODEL_H
#define SUBMODEL_H

#include "RenderableObject.h"

class SubModel : public RenderableObject
{
public:
	SubModel(Object* _parent, Mesh* _mesh, Material* _material) 
		:RenderableObject(_parent, _mesh, _material)
	{

	}
};

#endif