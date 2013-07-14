#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include"Mesh.h"
#include"Math.h"

class Cube : public Mesh
{
public:
	Cube(const wchar_t* _name)
		:Mesh(_name)
	{
		constructGeometryData();
	}

private:
	virtual void constructGeometryData();
};

//class Cylinder : public Mesh
//{
//public:
//	Cylinder(float _topRadius, float _bottomRadius, float _height, int _segmentsW, int _segmentsH, D3DXCOLOR _color)
//		:topRadius(_topRadius)
//		,bottomRadius(_bottomRadius)
//		,height(_height)
//		,segmentsW(_segmentsW)
//		,segmentsH(_segmentsH)
//		,color(_color)
//	{
//		constructGeometryData();
//	}
//
//private:
//	virtual void constructGeometryData();
//
//private:
//	float topRadius;
//	float bottomRadius;
//	float height;
//	int segmentsW;
//	int segmentsH;
//
//	D3DXCOLOR color;
//};
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