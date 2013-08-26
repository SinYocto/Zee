#ifndef DEBUGDRAWER_H
#define DEBUGDRAWER_H

#include "Common.h"
#include "Math.h"

class Camera;
class AABBox;

class Object;
class Mesh;

class TransGizmo
{
public:
	TransGizmo()
		:mCone(NULL)
		,mLine(NULL)
		,mRenderTarget(NULL)
		,mDepthStencil(NULL)
		,mSelected(SELECT_NONE)
	{

	}

	void Init();
	void Destroy();

	void Draw(Object* obj, Camera* camera);

private:
	void draw(Object* obj, Camera* camera, bool isColorPickPass);
	void getPickColor(D3DCOLOR* pickColor, const int pickPixelSize);
	void determinSelectType(Object* obj, Camera* camera);

private:
	enum SELECT_TYPE
	{
		SELECT_NONE,
		SELECT_X,
		SELECT_Y,
		SELECT_Z,
		SELECT_XY,
		SELECT_XZ,
		SELECT_YZ
	};

	enum GIZMO_COLOR
	{
		COLOR_NONE = 0xff000000,
		COLOR_X = 0xffff0000,
		COLOR_Y = 0xff00ff00,
		COLOR_Z = 0xff0000ff,

		COLOR_XY_PICK = 0xff00007f,
		COLOR_XZ_PICK = 0xff007f00,
		COLOR_YZ_PICK = 0xff7f0000,

		COLOR_XY = 0x5f00007f,
		COLOR_XZ = 0x5f007f00,
		COLOR_YZ = 0x5f7f0000,

		COLOR_SELECTED = 0xffffff00
	};

	Mesh* mCone;
	Mesh* mLine;

	IDirect3DSurface9* mRenderTarget;
	IDirect3DSurface9* mDepthStencil;

	SELECT_TYPE mSelected;
};


class DebugDrawer
{
public:
	enum LINE_TYPE
	{
		LINE_STRIP
	};

public:
	static bool DrawLine(const std::vector<Vector3>& points, D3DCOLOR color, Camera* camera, 
		const D3DXMATRIX& matWorld = IDENTITY_MATRIX, LINE_TYPE lineType = LINE_STRIP);

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