#include "CameraController.h"
#include "Camera.h"
#include "Input.h"

FPCameraController::FPCameraController(float moveSpeed, float rotateSpeed, float accelerateRatio)
:mMoveSpeed(moveSpeed)
,mRotateSpeed(rotateSpeed)
,mAccelerateRatio(accelerateRatio)
{

}

void FPCameraController::Apply(Camera* camera, float deltaTime)
{
	Vector3 moveVector = Vector3::Zero;
	float moveSpeed = mMoveSpeed;
	float rotateSpeed = mRotateSpeed;

	if(Input::GetKey(DIK_LSHIFT))
		moveSpeed *= mAccelerateRatio;

	if(Input::GetKey(DIK_W))
		moveVector += moveSpeed * deltaTime * Vector3(0, 0, 1);
	if(Input::GetKey(DIK_S))
		moveVector += moveSpeed * deltaTime * Vector3(0, 0, -1);

	if(Input::GetKey(DIK_A))
	{
		moveVector += moveSpeed * deltaTime * Vector3(-1, 0, 0);
	}
	if(Input::GetKey(DIK_D))
		moveVector += moveSpeed * deltaTime * Vector3(1, 0, 0);

	moveVector = camera->LocalVectorToWorld(moveVector);

	if(Input::GetKey(DIK_Q))
		moveVector += moveSpeed * deltaTime * Vector3(0, -1, 0);
	if(Input::GetKey(DIK_E))
		moveVector += moveSpeed * deltaTime * Vector3(0, 1, 0);


	if(moveVector != Vector3::Zero){
		camera->SetTransformDirty(true);
		camera->Translate(moveVector);
	}

	if(Input::mouseState.rgbButtons[1] & 0x80){
		float rotationY = rotateSpeed * Input::mouseState.lX / 1000.0f;
		float rotationX = rotateSpeed * Input::mouseState.lY / 1000.0f;

		if(rotationY != 0 || rotationX != 0){
			camera->SetTransformDirty(true);
			camera->Rotate(0, rotationY, 0);
			camera->RotateLocal(rotationX, 0, 0);
		}
	}
}


HoverCameraController::HoverCameraController(float rotateSpeed, float zoomSpeed, float minTiltAngle, float maxTiltAngle, 
											 float minDist, float maxDist)
											 :mRotateSpeed(rotateSpeed)
											 ,mZoomSpeed(zoomSpeed)
											 ,mMinTiltAngle(minTiltAngle)
											 ,mMaxTiltAngle(maxTiltAngle)
											 ,mMinDist(minDist)
											 ,mMaxDist(maxDist)
											 ,mTiltAngle(PI/4)
											 ,mPanAngle(0)
											 ,mDist(10.0f)
{

}

void HoverCameraController::Apply(Camera* camera, float deltaTime)
{
	if(Input::mouseState.rgbButtons[1] & 0x80){
		float deltaPanAngle = - mRotateSpeed * Input::mouseState.lX / 1000.0f;
		float deltaTiltAngle = mRotateSpeed * Input::mouseState.lY / 1000.0f;

		if(deltaPanAngle != 0 || deltaTiltAngle != 0)
			camera->SetTransformDirty(true);

		mPanAngle += deltaPanAngle;
		mTiltAngle += deltaTiltAngle;

		if(mTiltAngle > mMaxTiltAngle)
			mTiltAngle = mMaxTiltAngle;
		if(mTiltAngle < mMinTiltAngle)
			mTiltAngle = mMinTiltAngle;
	}

	if(Input::mouseState.rgbButtons[0] & 0x80){
		float deltaDistance = mZoomSpeed * ( -Input::mouseState.lX + Input::mouseState.lY) / 1000.0f;

		if(deltaDistance != 0)
			camera->SetTransformDirty(true);

		mDist += deltaDistance;
		if(mDist > mMaxDist)
			mDist = mMaxDist;
		if(mDist < mMinDist)
			mDist = mMinDist;
	}

	float y = mDist * sin(mTiltAngle);
	float x = mDist * cos(mTiltAngle) * cos(mPanAngle);
	float z = mDist * cos(mTiltAngle) * sin(mPanAngle);

	camera->SetWorldPosition(x, y, z);
	camera->LookAt(Vector3::Zero);
}
