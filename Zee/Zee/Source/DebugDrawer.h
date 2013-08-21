#ifndef DEBUGDRAWER_H
#define DEBUGDRAWER_H

#include "Common.h"
#include "Math.h"

class Camera;
class AABBox;

//class TransGizmo
//{
//public:
//	void Init();
//
//private:
//	Mesh* mCone;
//};

class DebugDrawer
{
public:
	enum LINE_TYPE
	{
		LINE_STRIP
	};

public:
	static bool DrawLine(const std::vector<Vector3>& points, D3DCOLOR color, Camera* camera, LINE_TYPE lineType = LINE_STRIP);

	static bool DrawSquare(const Vector3& center, const Vector3& normal, float size, 
		D3DCOLOR color, bool isSolid, Camera* camera);

	static bool DrawCircle(const Vector3& center, const Vector3& normal, float radius, D3DCOLOR color, bool isSolid,
		Camera* camera, int numSegments = 64);

	static bool DrawBox(const Vector3& center, const Quaternion& rotation, const Vector3& size, D3DCOLOR color, Camera* camera);
	static bool DrawAABBox(const AABBox& box, D3DCOLOR color, Camera* camera);

private:
	static bool drawSolidTriFan(const std::vector<Vector3>& points, D3DCOLOR color, Camera* camera);
};

#endif