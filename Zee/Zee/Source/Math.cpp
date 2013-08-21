#include"Math.h"
#include "Common.h"
#include"d3dx9.h"


bool FloatEqual(const float& f1, const float& f2, const float& tolerance)
{
	if(fabsf(f1 - f2) < fabsf(tolerance))
		return true;
	else 
		return false;
}


bool FloatUnequal(const float& f1, const float& f2, const float& tolerance)
{
	return !FloatEqual(f1, f2, tolerance);
}

void Clamp(float& val, const float min, const float max)
{
	if(val - max > FLT_MIN)
		val = max;

	if(min - val > FLT_MIN)
		val = min;
}

// Vector3
Vector3 Vector3::Zero = Vector3(0, 0, 0);

Vector3 operator+(const Vector3& vec1, const Vector3& vec2)
{ 
	Vector3 vec = vec1;
	vec += vec2;

	return vec;
}

Vector3 operator-(const Vector3& vec1, const Vector3& vec2)
{ 
	Vector3 vec = vec1;
	vec += -vec2;

	return vec;
}

Vector3 operator+(const Vector3& vec)
{
	return vec;
}

Vector3 operator-(const Vector3& vec)
{
	Vector3 vecResult;
	vecResult.x = -vec.x;
	vecResult.y = -vec.y;
	vecResult.z = -vec.z;

	return vecResult;
}

Vector3 operator*(float value, const Vector3& vec) 
{
	Vector3 vecResult = vec;
	vecResult *= value;

	return vecResult;
}

Vector3 Vector3::operator+=(const Vector3& vec)
{
	x += vec.x;
	y += vec.y;
	z += vec.z;

	return *this;
}

Vector3 Vector3::operator-=(const Vector3& vec)
{
	this->operator+=(-vec);

	return *this;
}

Vector3 Vector3::operator*=(float val)
{
	x *= val;
	y *= val;
	z *= val;

	return *this;
}

bool Vector3::operator==(const Vector3& vec) 
{ 
	return x == vec.x && y == vec.y && z == vec.z; 
}

bool Vector3::operator!=(const Vector3& vec) 
{ 
	return !(*this == vec); 
}

bool Vector3::operator<(const Vector3& vec)
{
	if(x < vec.x)
		return true;

	if(x > vec.x)
		return false;

	if(y < vec.y)
		return true;

	if(y > vec.y)
		return false;

	if(z < vec.z)
		return true;

	return false;
}

float Vector3::Length() const
{ 
	return sqrt(x*x + y*y + z*z);
}
	
void Vector3::Normalize() 
{ 
	float len = this->Length(); 
	if(len != 0)
	{
		x /= len; y /= len; z/= len; 
	}
}

Vector3 Vector3::Normalized() const
{
	Vector3 vec = *this;
	vec.Normalize();
	return vec;
}
	
float Vector3::Dot(Vector3 vec) const
{ 
	return x*vec.x + y*vec.y + z*vec.z; 
}
	
Vector3 Vector3::Cross(Vector3 vec) const
{ 
	// 因为xyz坐标轴属于左手系, 但向量叉乘和轴旋转又想用右手系判断, 所以加上负号
	return - Vector3(y*vec.z - z*vec.y, z*vec.x - x*vec.z, x*vec.y - y*vec.x);
}

float VectorAngle(const Vector3& vec1, const Vector3& vec2)
{
	float length1 = vec1.Length();
	float length2 = vec2.Length();

	_Assert(length1 != 0 && length2 != 0);

	float val = vec1.Dot(vec2) / (length1 * length2);
	Clamp(val, -1.0f, 1.0f);		// 若范围在-1 ~ 1外, acosf返回的是INFINITE

	return acosf(val);
}

float VectorLength(const Vector3& vec)
{
	return vec.Length();
}

bool Vector3Equal(const Vector3& vec1, const Vector3& vec2, const float& tolerance)
{
	if(FloatUnequal(vec1.x, vec2.x, tolerance))
		return false;

	if(FloatUnequal(vec1.y, vec2.y, tolerance))
		return false;

	if(FloatUnequal(vec1.z, vec2.z, tolerance))
		return false;

	return true;
}

bool Vector3Unequal(const Vector3& vec1, const Vector3& vec2, const float& tolerance)
{
	return !Vector3Equal(vec1, vec2, tolerance);
}

Vector2 operator+(const Vector2& vec1, const Vector2& vec2)
{ 
	Vector2 vec = vec1;
	vec.x += vec2.x;
	vec.y += vec2.y;

	return vec;
}

Vector2 operator-(const Vector2& vec1, const Vector2& vec2)
{ 
	Vector2 vec = vec1;
	vec.x -= vec2.x;
	vec.y -= vec2.y;

	return vec;
}

Vector2 operator+(const Vector2& vec)
{
	return vec;
}

Vector2 operator-(const Vector2& vec)
{
	Vector2 vecResult;
	vecResult.x = -vec.x;
	vecResult.y = -vec.y;

	return vecResult;
}

Vector2 operator*(float value, const Vector2& vec) 
{
	Vector2 vecResult = vec;
	vecResult.x *= value;
	vecResult.y *= value;

	return vecResult;
}


// Quaternion

// 欧拉角转成四元数
Quaternion::Quaternion(float eulerX, float eulerY, float eulerZ)
{
	float sx,sy,sz,cx,cy,cz;

	sx = sin(eulerX/2);
	cx = cos(eulerX/2);
	sy = sin(eulerY/2);
	cy = cos(eulerY/2);
	sz = sin(eulerZ/2);
	cz = cos(eulerZ/2);

	w = cy * cx * cz + sy * sx * sz;
	x = cy * sx * cz + sy * cx * sz;
	y = sy * cx * cz - cy * sx * sz;
	z = cy * cx * sz - sy * sx * cz;
}

// 矩阵转成四元数
Quaternion::Quaternion(D3DXMATRIX matrix)
{
	float m11 = matrix._11;
	float m12 = matrix._12;
	float m13 = matrix._13;

	float m21 = matrix._21;
	float m22 = matrix._22;
	float m23 = matrix._23;

	float m31 = matrix._31;
	float m32 = matrix._32;
	float m33 = matrix._33;

	float tw = m11 + m22 + m33;
	float tx = m11 - m22 - m33;
	float ty = m22 - m11 - m33;
	float tz = m33 - m11 - m22;

	int biggestIx = 0;
	float tb = tw;
	if(tx > tb)
	{
		tb = tx;
		biggestIx = 1;
	}
	if(ty > tb)
	{
		tb = ty;
		biggestIx = 2;
	}
	if(tz > tb)
	{
		tb = tz;
		biggestIx = 3;
	}

	float biggestVal = sqrt(tb + 1) * 0.5f;
	float mult = 0.25f / biggestVal;

	switch(biggestIx)
	{
	case 0:
		w = biggestVal;
		x = (m23 - m32) * mult;
		y = (m31 - m13) * mult;
		z = (m12 - m21) * mult;
		break;

	case 1:
		x = biggestVal;
		w = (m23 - m32) * mult;
		y = (m12 + m21) * mult;
		z = (m31 + m13) * mult;
		break;

	case 2:
		y = biggestVal;
		w = (m31 - m13) * mult;
		x = (m12 + m21) * mult;
		z = (m23 + m32) * mult;
		break;

	case 3:
		z = biggestVal;
		w = (m12 - m21) * mult;
		x = (m31 + m13) * mult;
		y = (m23 + m32) * mult;
		break;
	}
}

// 四元数乘法
Quaternion operator*(const Quaternion& q1, const Quaternion& q2)
{
	Quaternion q;
	q.w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;
	q.x = q1.y * q2.z - q1.z * q2.y + q1.w * q2.x + q1.x * q2.w;
	q.y = q1.z * q2.x - q1.x * q2.z + q1.w * q2.y + q1.y * q2.w;
	q.z = q1.x * q2.y - q1.y * q2.x + q1.w * q2.z + q1.z * q2.w;

	return q;
}

// 点或向量乘以四元数（点或向量经四元数变换）
Vector3 operator*(const Vector3& vec, const Quaternion& q)
{
	Quaternion vecQ = Quaternion(0, vec.x, vec.y, vec.z);

	// 注意坐标使用的是左手系, 旋转判断使用的左手系(顺时针为正), 四元数是左乘
	Quaternion resultQ = q * vecQ * q.Conjugate();

	return Vector3(resultQ.x, resultQ.y, resultQ.z);
}

// 四元数的共轭
Quaternion Quaternion::Conjugate() const
{
	return Quaternion(w, -x, -y, -z);
}

// 四元数转成欧拉角
Vector3 Quaternion::EulerAngle()
{
	float p,h,b;
	float sp = -2.0f * (y*z + w*x);

	if(fabs(sp) > 0.9999f){
		p = PI/2;
		h = atan2(-x*z + w*y, 0.5f - y*y - z*z);
		b = 0.0f;
	}
	else{
		p = asin(sp);
		h = atan2(x*z + w*y, 0.5f - x*x - y*y);
		b = atan2(x*y + w*z, 0.5f - x*x - z*z);
	}

	return Vector3(p,h,b);
}

// 四元数转成矩阵
D3DXMATRIX Quaternion::Matrix()
{
	D3DXMATRIX mat;
	D3DXMatrixIdentity(&mat);

	mat._11 = 1 - 2*(y*y + z*z);
	mat._12 = 2*(x*y + w*z);
	mat._13 = 2*(x*z - w*y);

	mat._21 = 2*(x*y - w*z);
	mat._22 = 1 - 2*(x*x + z*z);
	mat._23 = 2*(y*z + w*x);

	mat._31 = 2*(x*z + w*y);
	mat._32 = 2*(y*z - w*x);
	mat._33 = 1 - 2*(x*x +y*y);

	return mat;
}

// 本四元数到quat间的difference
// d*a = b, d为a到b间的difference
Quaternion Quaternion::Difference(Quaternion quat)
{
	return quat * this->Conjugate();
}

void Quaternion::Normalize()
{
	float length = sqrt(w*w + x*x + y*y + z*z);
	if(FloatUnequal(length, 0, 0.00001f))
	{
		w /= length;
		x /= length;
		y /= length;
		z /= length;
	}
}

Quaternion Quaternion::VectorRotation(Vector3 vec1, Vector3 vec2)
{
	vec1.Normalize();
	vec2.Normalize();

	float dot = vec1.Dot(vec2);

	if(FloatEqual(dot, 1, 0.0001f))
	{
		return Quaternion(1, 0, 0, 0);
	}

	if(FloatEqual(dot, -1, 0.0001f))
	{
		return Quaternion(0, 0, 1, 0);
	}

	Vector3 axis = vec1.Cross(vec2);

	Quaternion rotation;
	rotation.x = axis.x;
	rotation.y = axis.y;
	rotation.z = axis.z;
	rotation.w = 1 + dot;

	rotation.Normalize();

	return rotation;
}

Rect::Rect()
{
	left = 0;
	top = 0;
	right = 0;
	bottom = 0;
}

Rect::Rect(long _left, long _top, long width, long height)
{
	left = _left;
	top = _top;
	right = left + width;
	bottom = top + height;
}

void Rect::TransLate(int x, int y)
{
	left += x;
	right += x;
	top += y;
	bottom += y;
}
