#ifndef BOUND_H
#define BOUND_H

#include"Math.h"

class BoundingBox
{
public:
	BoundingBox();
	BoundingBox(Vector3 _max, Vector3 _min);
	BoundingBox(Vector3 center, float halfSizeX, float halfSizeY, float halfSizeZ);
	Vector3 GetVertex(int ix);
	bool isPointInside(Vector3 vert);
public:
	Vector3 max;
	Vector3 min;
};


#endif