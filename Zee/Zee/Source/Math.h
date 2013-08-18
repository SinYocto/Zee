#ifndef MATH_H
#define MATH_H

#include"d3dx9.h"
#include<math.h>

#define PI 3.141592654f

bool FloatEqual(const float& f1, const float& f2, const float& tolerance);
bool FloatUnequal(const float& f1, const float& f2, const float& tolerance);
void Clamp(float& val, const float min, const float max);

class Vector3
{
public:
	Vector3(float _x = 0, float _y = 0, float _z = 0)
		:x(_x)
		,y(_y)
		,z(_z)
	{

	}

	struct Comparer
	{
		bool operator()(const Vector3& vec1, const Vector3& vec2) const
		{
			if(vec1.x < vec2.x)
				return true;

			if(vec1.x > vec2.x)
				return false;

			if(vec1.y < vec2.y)
				return true;

			if(vec1.y > vec2.y)
				return false;

			if(vec1.z < vec2.z)
				return true;

			return false;
		}
	};

	bool operator==(const Vector3& vec); 
	bool operator!=(const Vector3& vec);
	bool operator<(const Vector3& vec);
	Vector3 operator+=(const Vector3& vec); 
	Vector3 operator-=(const Vector3& vec); 
	Vector3 operator*=(float val);

	float Length() const;
	void Normalize();
	Vector3 Normalized() const;
	float Dot(Vector3 vec) const;
	Vector3 Cross(Vector3 vec) const;

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

float VectorAngle(const Vector3& vec1, const Vector3& vec2);
float VectorLength(const Vector3& vec);

bool Vector3Equal(const Vector3& vec1, const Vector3& vec2, const float& tolerance);
bool Vector3Unequal(const Vector3& vec1, const Vector3& vec2, const float& tolerance);

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
Vector2 operator+(const Vector2& vec);
Vector2 operator-(const Vector2& vec);
Vector2 operator+(const Vector2& vec1, const Vector2& vec2);
Vector2 operator-(const Vector2& vec1, const Vector2& vec2);
Vector2 operator*(float value, const Vector2& vec);

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

	void Normalize();

	Vector3 EulerAngle();
	D3DXMATRIX Matrix();

	static Quaternion VectorRotation(Vector3 vec1, Vector3 vec2);

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

	void TransLate(int x, int y);
public:
	long left, top, right, bottom;
};

#endif