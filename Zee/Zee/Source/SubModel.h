#include "RenderableObject.h"

class SubModel : public RenderableObject
{
public:
	SubModel(Object* _parent, Mesh* _mesh = NULL, Material* _material = NULL) 
		:RenderableObject(_parent, _mesh, _material)
	{

	}
};