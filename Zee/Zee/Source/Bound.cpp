#include"Bound.h"

BoundingBox::BoundingBox() {}

BoundingBox::BoundingBox(Vector3 _max, Vector3 _min)
{
	max = _max;
	min = _min;
}

BoundingBox::BoundingBox(Vector3 center, float halfSizeX, float halfSizeY, float halfSizeZ)
{
	Vector3 halfSize(halfSizeX, halfSizeY, halfSizeZ);
	max = center + halfSize;
	min = center - halfSize;
}

Vector3 BoundingBox::GetVertex(int ix)
{
	Vector3 vert;
	switch(ix){
	case 0:
		vert = min;
		break;
	case 1:
		vert = Vector3(min.x, min.y, max.z);
		break;
	case 2:
		vert = Vector3(max.x, min.y, max.z);
		break;
	case 3:
		vert = Vector3(max.x, min.y, min.z);
		break;
	case 4:
		vert = Vector3(min.x, max.y, min.z);
		break;
	case 5:
		vert = Vector3(min.x, max.y, max.z);
		break;
	case 6:
		vert = max;
		break;
	case 7:
		vert = Vector3(max.x, max.y, min.z);
		break;
	}

	return vert;
}

bool BoundingBox::isPointInside(Vector3 vert)
{
	if(vert.x > min.x && vert.x < max.x &&
		vert.y > min.y && vert.y < max.y &&
		vert.z > min.z && vert.z < max.z)
		return true;
	else 
		return false;
}