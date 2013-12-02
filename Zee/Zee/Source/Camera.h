#ifndef CAMERA_H
#define CAMERA_H

#include "Object.h"
#include "Bound.h"

class CameraController;

struct DampContext
{
	DampContext()
		:isDamping(false)
		,dampVel(Vector3::Zero)
		,dampAcc(Vector3::Zero)
		,targetPos(Vector3::Zero)
		,totalTime(0)
		,elapsedTime(0)
	{

	}

	DampContext(const Vector3& fromPos, const Vector3& toPos, float time, float dampFactor)
	{
		isDamping = true;

		Clamp(dampFactor, 0, 1.0f);

		float s = VectorLength(fromPos - toPos);
		float f = dampFactor;
		float t = time;

		// 匀变速直线运动, 已知时间和位移解出来的初速度和加速度
		float v0 = 2 * (1 + f) * s * (1 + sqrtf(f / (1+f))) / t;
		float a = 2 * (v0 * t - s) / (t*t);

		Vector3 velDir = toPos - fromPos;
		velDir.Normalize();

		dampVel = v0 * velDir;
		dampAcc = - a * velDir;

		elapsedTime = 0;
		totalTime = time;
		targetPos = toPos;
	}

	void Clear()
	{
		isDamping = false;
		dampVel = Vector3::Zero;
		dampAcc = Vector3::Zero;
		elapsedTime = 0;
		totalTime = 0;
	}

	bool isDamping;

	Vector3 dampVel;
	Vector3 dampAcc;

	Vector3 targetPos;

	float elapsedTime;
	float totalTime;
};

class Camera : public Object
{
public:
	Camera(const Vector3 pos = Vector3(0, 0, -200), const Vector3 target = Vector3::Zero, 
		float fov = PI/2, float aspect = 1.0f, float nZ = 1.0f, float fZ = 1000.0f)
		:Object(NULL, pos)
		,mFOV(fov)
		,mAspect(aspect)
		,mNearZ(nZ)
		,mFarZ(fZ)
		,mController(NULL)
	{
		LookAt(target);
		recalculateCameraMatrix();
		extractFrustumPlanes();

		mIsTranformDirty = false;
		mIsParametersDirty = false;
	}

	~Camera();

	D3DXMATRIX ViewMatrix() const;
	D3DXMATRIX ProjMatrix() const;

	D3DXMATRIX OrthoProjMatrix(float orthoWidth) const;

	bool IsVisible(AABBox boundingBox);

	void SetAspect(float aspect);

	void SetTransformDirty(bool isDirty);
	void SetParametersDirty(bool isDirty);

	void FrameUpdate();

	void GetScreenRay(const Vector2& screenPos, Vector3* rayPos, Vector3* rayDir);

	void GetCameraParams(float* nearZ, float* farZ, float* fov, float* aspect);

	void SetCameraController(CameraController* controller);
	void ApplyCameraController();

	void DampMoveTo(const Vector3& targetPos, float time, float dampFactor);
	void FocusAt(Object* obj);

	virtual void OnTransformChanged();

private:
	void recalculateCameraMatrix();
	void recalculateViewMatrix();
	void recalculateProjMatrix();

	void extractFrustumPlanes();

private:
	float mFOV;
	float mAspect;
	float mNearZ;
	float mFarZ;

	D3DXPLANE mFrustumPlanes[6];

	D3DXMATRIX mMatProj;
	D3DXMATRIX mMatView;

	bool mIsTranformDirty;
	bool mIsParametersDirty;

	CameraController* mController;

	DampContext mDampContext; 
};



#endif