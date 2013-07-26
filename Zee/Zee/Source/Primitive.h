#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include"Geometry.h"
#include"Math.h"

class Cube : public Geometry
{
public:
	Cube(const wchar_t* _name)
		:Geometry(_name)
	{
		constructGeometryData();
	}

private:
	virtual void constructGeometryData();
};

class Cylinder : public Geometry
{
public:
	Cylinder(const wchar_t* _name, float _topRadius, float _bottomRadius, float _height, int _segmentsW = 32, int _segmentsH = 4)
		:Geometry(_name)
		,topRadius(_topRadius)
		,bottomRadius(_bottomRadius)
		,height(_height)
		,segmentsW(_segmentsW)
		,segmentsH(_segmentsH)
	{
		constructGeometryData();
	}

private:
	virtual void constructGeometryData();

private:
	float topRadius;
	float bottomRadius;
	float height;
	int segmentsW;
	int segmentsH;
};
//
//class Sphere : public Mesh
//{
//public:
//	Sphere(float _radius, int _segmentsW, int _segmentsH)
//		:radius(_radius)
//		,segmentsW(_segmentsW)
//		,segmentsH(_segmentsH)
//	{
//		constructGeometryData();
//	}
//
//private:
//	virtual void constructGeometryData();
//
//private:
//	float radius;
//	int segmentsW;
//	int segmentsH;
//};

#endif