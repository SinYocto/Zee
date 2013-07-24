#ifndef DEBUGDRAWER_H
#define DEBUGDRAWER_H

#include "Utility.h"
#include "Math.h"

class Camera;

class DebugDrawer
{
public:
	enum LineType
	{
		LINE_STRIP
	};

public:
	static bool DrawLine(const std::vector<Vector3>& points, D3DCOLOR color, Camera* camera, LineType lineType = LINE_STRIP);
	static bool DrawSquare(const Vector3& center, const Vector3& normal, float size, D3DCOLOR color, Camera* camera);
	static bool DrawCircle(const Vector3& center, const Vector3& normal, float radius, D3DCOLOR color, 
		Camera* camera, int numSegments = 64);
};

#endif