#include"Bound.h"
#include "Common.h"

AABBox AABBox::Invalid(Vector3(FLT_MAX, FLT_MAX, FLT_MAX), Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX));

AABBox::AABBox(const Vector3& min /*= Vector3(FLT_MAX, FLT_MAX, FLT_MAX)*/, 
			   const Vector3& max /*= Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX)*/)
{
	mMin = min;
	mMax = max;
}

AABBox::AABBox(const Vector3& center, float width, float height, float length)
{
	Vector3 size(width, height, length);

	mMax = center + 0.5f * size;
	mMin = center - 0.5f * size;
}

Vector3 AABBox::GetVertex(int ix)
{
	Vector3 vert;
	switch(ix){
	case 0:
		vert = mMin;
		break;
	case 1:
		vert = Vector3(mMin.x, mMin.y, mMax.z);
		break;
	case 2:
		vert = Vector3(mMax.x, mMin.y, mMax.z);
		break;
	case 3:
		vert = Vector3(mMax.x, mMin.y, mMin.z);
		break;
	case 4:
		vert = Vector3(mMin.x, mMax.y, mMin.z);
		break;
	case 5:
		vert = Vector3(mMin.x, mMax.y, mMax.z);
		break;
	case 6:
		vert = mMax;
		break;
	case 7:
		vert = Vector3(mMax.x, mMax.y, mMin.z);
		break;
	}

	return vert;
}

bool AABBox::isPointInside(Vector3 vert)
{
	if(vert.x > mMin.x && vert.x < mMax.x &&
		vert.y > mMin.y && vert.y < mMax.y &&
		vert.z > mMin.z && vert.z < mMax.z)
		return true;
	else 
		return false;
}

AABBox AABBox::CombineBBox(const AABBox& box1, const AABBox& box2)
{
	AABBox resultBox = box1;

	if(box2.mMin.x < resultBox.mMin.x)
		resultBox.mMin.x = box2.mMin.x;

	if(box2.mMin.y < resultBox.mMin.y)
		resultBox.mMin.y = box2.mMin.y;

	if(box2.mMin.z < resultBox.mMin.z)
		resultBox.mMin.z = box2.mMin.z;

	if(box2.mMax.x > resultBox.mMax.x)
		resultBox.mMax.x = box2.mMax.x;

	if(box2.mMax.y > resultBox.mMax.y)
		resultBox.mMax.y = box2.mMax.y;

	if(box2.mMax.z > resultBox.mMax.z)
		resultBox.mMax.z = box2.mMax.z;

	return resultBox;
}

AABBox AABBox::Intersection(const AABBox& box1, const AABBox& box2)
{
	AABBox resultBox = box1;

	if(box2.mMin.x > resultBox.mMin.x)
		resultBox.mMin.x = box2.mMin.x;

	if(box2.mMin.y > resultBox.mMin.y)
		resultBox.mMin.y = box2.mMin.y;

	if(box2.mMin.z > resultBox.mMin.z)
		resultBox.mMin.z = box2.mMin.z;

	if(box2.mMax.x < resultBox.mMax.x)
		resultBox.mMax.x = box2.mMax.x;

	if(box2.mMax.y < resultBox.mMax.y)
		resultBox.mMax.y = box2.mMax.y;

	if(box2.mMax.z < resultBox.mMax.z)
		resultBox.mMax.z = box2.mMax.z;

	return resultBox;
}

bool AABBox::isValid()
{
	if(mMin.x > mMax.x)
		return false;

	if(mMin.y > mMax.y)
		return false;

	if(mMin.z > mMax.z)
		return false;

	return true;
}

Vector3 AABBox::GetCenter()
{
	_Assert(isValid());

	return 0.5f * (mMin + mMax);
}

AABBox AABBox::CombinePoint(const AABBox& box, const Vector3& point)
{
	AABBox resultBox = box;

	if(point.x < resultBox.mMin.x)
		resultBox.mMin.x = point.x;

	if(point.y < resultBox.mMin.y)
		resultBox.mMin.y = point.y;

	if(point.z < resultBox.mMin.z)
		resultBox.mMin.z = point.z;

	if(point.x > resultBox.mMax.x)
		resultBox.mMax.x = point.x;

	if(point.y > resultBox.mMax.y)
		resultBox.mMax.y = point.y;

	if(point.z > resultBox.mMax.z)
		resultBox.mMax.z = point.z;

	return resultBox;

}

AABBox AABBox::MatTransform(AABBox box, const D3DXMATRIX& mat)
{
	AABBox resultBox;
	
	for(int i = 0; i < 8; ++i)
	{
		Vector3 vertPos = box.GetVertex(i);
		vertPos = PosVecTransform(vertPos, mat);

		resultBox = CombinePoint(resultBox, vertPos);
	}

	return resultBox;
}

bool AABBox::IntersectedWith(const AABBox& box)
{
	AABBox intersect = Intersection(*this, box);
	return intersect.isValid();
}

Vector3 AABBox::GetSize()
{
	_Assert(isValid());

	return mMax - mMin;
}

Ray::Ray(const Vector3& pos /*= Vector3::Zero*/, const Vector3& dir /*= Vector3(0, 0, 1)*/)
{
	mPos = pos;
	mDir = dir;
}

bool IntersectRayAABB(const Vector3& rayPos, const Vector3& rayDir, const AABBox& box, Vector3* hitPos, float* dist)
{
//	_Assert(NULL != hitPos);
//	_Assert(NULL != dist);

	float tMin = 0;
	float tMax = FLT_MAX;

	float rp[3] = { rayPos.x, rayPos.y, rayPos.z };
	float rd[3] = { rayDir.x, rayDir.y, rayDir.z };

	float boxMin[3] = { box.mMin.x, box.mMin.y, box.mMin.z };
	float boxMax[3] = { box.mMax.x, box.mMax.y, box.mMax.z };

	for(int i = 0; i < 3; ++i)
	{
		if(fabs(rd[i]) < FLT_EPSILON)
		{
			if(rp[i] < boxMin[i] || rp[i] > boxMax[i])
				return false;
		}
		else
		{
			float t1 = (boxMin[i] - rp[i]) / rd[i];
			float t2 = (boxMax[i] - rp[i]) / rd[i];

			if(t1 > t2)
				std::swap(t1, t2);

			if(t1 > tMin)
				tMin = t1;

			if(t2 < tMax)
				tMax = t2;

			if(tMin > tMax)
				return false;
		}
	}

	Vector3 hitP = rayPos + tMin * rayDir;
	float d = VectorLength(rayPos - hitP);

	if(hitPos)
		*hitPos = hitP;

	if(dist)
		*dist = d;

	return true;
}

bool IntersectRayPlane(const Vector3& rayPos, const Vector3& rayDir, const D3DXPLANE& plane, Vector3* hitPos, float* dist)
{
	Vector3 normal(plane.a, plane.b, plane.c);

	if(rayDir.Dot(normal) == 0)
		return false;

	float t = -(rayPos.Dot(normal) + plane.d) / (rayDir.Dot(normal));
	Vector3 hitP = rayPos + t * rayDir;

	if(hitPos)
		*hitPos = hitP;

	if(dist)
		*dist = VectorLength(rayPos - hitP);

	return true;
}

bool IsPointInsideTriangle(const Vector3& point, const Vector3& triVertA, const Vector3& triVertB, const Vector3& triVertC)
{
	// http://www.cnblogs.com/graphics/archive/2010/08/05/1793393.html Í¬Ïò·¨
	bool inside = true;

	Vector3 vecAB = triVertB - triVertA;
	Vector3 vecAC = triVertC - triVertA;
	Vector3 vecBC = triVertC - triVertB;

	Vector3 vecAP = point - triVertA;
	Vector3 vecBP = point - triVertB;
	Vector3 vecCP = point - triVertC;

	if((vecAB.Cross(vecAP)).Dot((vecAB.Cross(vecAC))) < 0)
		inside = false;

	if((vecBC.Cross(vecBP)).Dot((vecBC.Cross(-vecAB))) < 0)
		inside = false;

	if((vecAC.Cross(-vecCP)).Dot((vecAC.Cross(vecBC))) < 0)
		inside = false;

	return inside;
}

bool IntersectRayTriangle(const Vector3& rayPos, const Vector3& rayDir, const Vector3& triVertA, const Vector3& triVertB, 
						  const Vector3& triVertC, Vector3* hitPos, float* dist)
{
	D3DXPLANE plane;
	D3DXPlaneFromPoints(&plane, (D3DXVECTOR3*)&triVertA, (D3DXVECTOR3*)&triVertB, (D3DXVECTOR3*)&triVertC);

	Vector3 hitP;
	float hitDist = 0;
	if(IntersectRayPlane(rayPos, rayDir, plane, &hitP, &hitDist))
	{
		if(IsPointInsideTriangle(hitP, triVertA, triVertB, triVertC))
		{
			if(hitPos)
				*hitPos = hitP;

			if(dist)
				*dist = hitDist;

			return true;
		}

		ConsolePrint(L"not hit tir, hitpos(%f,%f,%f)\n", hitP.x, hitP.y, hitP.z);
	}

	return false;
}
