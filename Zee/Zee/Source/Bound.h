#ifndef BOUND_H
#define BOUND_H

#include"Math.h"

class Ray
{
public:
	Ray(const Vector3& pos = Vector3::Zero, const Vector3& dir = Vector3(0, 0, 1));

public:
	Vector3 mPos;
	Vector3 mDir;
};

class AABBox
{
public:
	AABBox(const Vector3& min = Vector3(FLT_MAX, FLT_MAX, FLT_MAX), const Vector3& max = Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX));
	AABBox(const Vector3& center, float width, float height, float length);

	Vector3 GetVertex(int ix);
	Vector3 GetCenter();
	Vector3 GetSize();
	bool isPointInside(Vector3 vert);
	bool isValid();
	bool IntersectedWith(const AABBox& box);

	static AABBox CombineBBox(const AABBox& box1, const AABBox& box2);
	static AABBox CombinePoint(const AABBox& box, const Vector3& point);
	static AABBox Intersection(const AABBox& box1, const AABBox& box2);

	static AABBox MatTransform(AABBox box, const D3DXMATRIX& mat);

public:
	Vector3 mMin;
	Vector3 mMax;

	static AABBox Invalid;
};

bool IntersectRayAABB(const Vector3& rayPos, const Vector3& rayDir, const AABBox& box, Vector3* hitPos, float* dist);
bool IntersectRayPlane(const Vector3& rayPos, const Vector3& rayDir, const D3DXPLANE& plane, Vector3* hitPos, float* dist);

#endif