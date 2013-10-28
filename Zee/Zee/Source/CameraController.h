#ifndef CAMERA_CONTROLLER_H
#define CAMERA_CONTROLLER_H

class Camera;

class CameraController
{
public:
	virtual void Apply(Camera* camera, float deltaTime) = 0;
};

class FPCameraController : public CameraController
{
public:
	FPCameraController(float moveSpeed, float rotateSpeed, float accelerateRatio);
	virtual void Apply(Camera* camera, float deltaTime);

private:
	float mMoveSpeed;
	float mRotateSpeed;
	float mAccelerateRatio;
};

class HoverCameraController : public CameraController
{
public:
	HoverCameraController(float rotateSpeed, float zoomSpeed, float minTiltAngle, float maxTiltAngle, 
		float minDist, float maxDist);

	virtual void Apply(Camera* camera, float deltaTime);

private:
	float mRotateSpeed;
	float mZoomSpeed;
	float mMinTiltAngle;
	float mMaxTiltAngle;
	float mMinDist;
	float mMaxDist;

	float mTiltAngle;
	float mPanAngle;
	float mDist;
};

#endif