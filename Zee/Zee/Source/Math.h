#ifndef MATH_H
#define MATH_H

#include"d3dx9.h"
#include<math.h>

#define PI 3.141592654f

class Vector3
{
public:
	Vector3(float _x = 0, float _y = 0, float _z = 0)
		:x(_x)
		,y(_y)
		,z(_z)
	{

	}

	bool operator==(const Vector3& vec); 
	bool operator!=(const Vector3& vec);
	Vector3 operator+=(const Vector3& vec); 
	Vector3 operator-=(const Vector3& vec); 
	Vector3 operator*=(float val);

	float Length();
	void Normalize();
	Vector3 Normalized();
	float Dot(Vector3 vec);
	Vector3 Cross(Vector3 vec);

public:
	float x;
	float y;
	float z;

	static Vector3 Zero;
};
Vector3 operator+(const Vector3& vec);
Vector3 operator-(const Vector3& vec);
Vector3 operator+(const Vector3& vec1, const Vector3& vec2);
Vector3 operator-(const Vector3& vec1, const Vector3& vec2);
Vector3 operator*(float value, const Vector3& vec);

class Vector2
{
public:
	Vector2(float _x = 0, float _y = 0)
		:x(_x)
		,y(_y)
	{

	}

public:
	float x;
	float y;
};

class Vector4
{
public:
	Vector4(float _x = 0, float _y = 0, float _z = 0, float _w = 0)
		:x(_x)
		,y(_y)
		,z(_z)
		,w(_w)
	{

	}

public:
	float x;
	float y;
	float z;
	float w;
};

class Quaternion
{
public:
	Quaternion()
		:w(1.0f)
		,x(0)
		,y(0)
		,z(0)
	{

	}

	Quaternion(float _w, float _x, float _y, float _z)
		:w(_w)
		,x(_x)
		,y(_y)
		,z(_z)
	{

	}

	Quaternion(float eulerX, float eulerY, float eulerZ);
	Quaternion(D3DXMATRIX matrix);

	Quaternion Conjugate() const;

	Quaternion Difference(Quaternion quat);

	Vector3 EulerAngle();
	D3DXMATRIX Matrix();

public:
	float w;
	float x;
	float y;
	float z;
};
Quaternion operator*(const Quaternion& q1, const Quaternion& q2);
Vector3 operator*(const Vector3& vec, const Quaternion& q);

class Rect
{
public:
	Rect();
	Rect(long _left, long _top, long width, long height);
public:
	long left, top, right, bottom;
};

#endif