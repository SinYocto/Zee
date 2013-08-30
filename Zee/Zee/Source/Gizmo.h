#ifndef GIZMO_H
#define GIZMO_H

#include "DebugDrawer.h"
#include "Mesh.h"

class Camera;

class Object;
class Mesh;
class Material;

const float SCALE_FACTOR = 6.0f;
const float TRANS_SPEED = 8.0f;
const float ROTATE_SPEED = 2.0f;
const float SCALE_SPEED = 8.0f;

class Gizmo
{
public:
	enum GIZMO_TYPE
	{
		GIZMO_NONE,
		GIZMO_TRANS,
		GIZMO_ROTATE,
		GIZMO_SCALE
	};

private:
	enum SELECT_TYPE
	{
		AXIS_X = 0,
		AXIS_Y = 1,
		AXIS_Z = 2,
		PLANE_XY = 3,
		PLANE_XZ = 4,
		PLANE_YZ = 5,
		SELECT_NONE
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

public:
	Gizmo()
		:mCone(NULL)
		,mLine(NULL)
		,mTorus(NULL)
		,mCube(NULL)
		,mRenderTarget(NULL)
		,mDepthStencil(NULL)
		,mSelected(SELECT_NONE)
		,mActiveType(GIZMO_NONE)
	{

	}

	void Init();
	void Destroy();

	void OnLostDevice();
	void OnResetDevice();

	void SetActiveType(GIZMO_TYPE type);
	void Draw(Object* obj, Camera* camera);

	bool IsSelected();

private:
	void draw(Object* obj, Camera* camera, bool isColorPickPass);

	void drawTransGizmo(Object* obj, Camera* camera, Material* mtl, D3DCOLOR elementsColor[6]);
	void drawRotateGizmo(Object* obj, Camera* camera, Material* mtl, D3DCOLOR elementsColor[6]);
	void drawScaleGizmo(Object* obj, Camera* camera, Material* mtl, D3DCOLOR elementsColor[6]);

	void getPickColor(D3DCOLOR* pickColor, const int pickPixelSize);
	void determinSelectType(Object* obj, Camera* camera);

	void applyTrans(Object* obj, Camera* camera, const Vector3& tangentX, const Vector3& tangentY, const Vector3& tangentZ);
	void applyRotate(Object* obj, Camera* camera, const Vector3& tangent);
	void applyScale(Object* obj, Camera* camera, const Vector3& tangentX, const Vector3& tangentY, const Vector3& tangentZ);

	void calTransTangent(Object* obj, Camera* camera, Vector3* tangentX, Vector3* tangentY, Vector3* tangentZ);
	void calRotateTangent(Object* obj, Camera* camera, Vector3* tangent);

	void createRenderTargetDepthStencile();

private:
	Mesh* mCone;
	Mesh* mLine;
	Mesh* mTorus;
	Mesh* mCube;

	IDirect3DSurface9* mRenderTarget;
	IDirect3DSurface9* mDepthStencil;

	SELECT_TYPE mSelected;
	GIZMO_TYPE mActiveType;
};

#endif