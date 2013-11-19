#include "CameraController.h"
#include "Camera.h"
#include "Engine.h"

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

	Input* input = gEngine->GetInput();

	if(input->GetKey(DIK_LSHIFT))
		moveSpeed *= mAccelerateRatio;

	if(input->GetKey(DIK_W))
		moveVector += moveSpeed * deltaTime * Vector3(0, 0, 1);
	if(input->GetKey(DIK_S))
		moveVector += moveSpeed * deltaTime * Vector3(0, 0, -1);

	if(input->GetKey(DIK_A))
	{
		moveVector += moveSpeed * deltaTime * Vector3(-1, 0, 0);
	}
	if(input->GetKey(DIK_D))
		moveVector += moveSpeed * deltaTime * Vector3(1, 0, 0);

	moveVector = camera->LocalVectorToWorld(moveVector);

	if(input->GetKey(DIK_Q))
		moveVector += moveSpeed * deltaTime * Vector3(0, -1, 0);
	if(input->GetKey(DIK_E))
		moveVector += moveSpeed * deltaTime * Vector3(0, 1, 0);


	if(moveVector != Vector3::Zero){
		camera->SetTransformDirty(true);
		camera->Translate(moveVector);
	}

	if(input->GetRightButton()){
		DIMOUSESTATE mouseState = input->GetMouseState();

		float rotationY = rotateSpeed * mouseState.lX / 1000.0f;
		float rotationX = rotateSpeed * mouseState.lY / 1000.0f;

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
											 ,mTiltAngle(0)
											 ,mPanAngle(-PI/2)
											 ,mDist(10.0f)
{

}

void HoverCameraController::Apply(Camera* camera, float deltaTime)
{
	Input* input = gEngine->GetInput();

	if(input->GetRightButton()){
		DIMOUSESTATE mouseState = input->GetMouseState();

		float deltaPanAngle = - mRotateSpeed * mouseState.lX / 1000.0f;
		float deltaTiltAngle = mRotateSpeed * mouseState.lY / 1000.0f;

		if(deltaPanAngle != 0 || deltaTiltAngle != 0)
			camera->SetTransformDirty(true);

		mPanAngle += deltaPanAngle;
		mTiltAngle += deltaTiltAngle;

		if(mTiltAngle > mMaxTiltAngle)
			mTiltAngle = mMaxTiltAngle;
		if(mTiltAngle < mMinTiltAngle)
			mTiltAngle = mMinTiltAngle;
	}

	if(input->GetLeftButton()){
		DIMOUSESTATE mouseState = input->GetMouseState();

		float deltaDistance = mZoomSpeed * ( -mouseState.lX + mouseState.lY) / 1000.0f;

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
