#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include"Geometry.h"

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

class Torus : public Geometry
{
public:
	Torus(const wchar_t* name, float radius, float tubeRadius, int segmentsR = 32, int segmentsT = 24)
		:Geometry(name)
		,mRadius(radius)
		,mTubeRadius(tubeRadius)
		,mSegmentsR(segmentsR)
		,mSegmentsT(segmentsT)
	{
		constructGeometryData();
	}

private:
	virtual void constructGeometryData();

private:
	float mRadius;
	float mTubeRadius;
	int mSegmentsR;
	int mSegmentsT;
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