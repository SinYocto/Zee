#include "Camera.h"
#include "Driver.h"
#include "CameraController.h"
#include "Engine.h"

Camera::~Camera()
{
	SAFE_DELETE(mController);
}

void Camera::recalculateViewMatrix()
{
	D3DXMATRIX matRotTranspose, matTransInverse;
	D3DXMatrixTranspose(&matRotTranspose, &(mRelativeOrient.Matrix()));
	D3DXMatrixTranslation(&matTransInverse, -mRelativePos.x, -mRelativePos.y, -mRelativePos.z);
	mMatView = matTransInverse * matRotTranspose; 
}

void Camera::recalculateProjMatrix()
{
	D3DXMatrixPerspectiveFovLH(&mMatProj, mFOV, mAspect, mNearZ, mFarZ);
}

void Camera::extractFrustumPlanes()
{
	D3DXMATRIX matVP = ViewMatrix() * ProjMatrix();

	D3DXVECTOR4 col0(matVP(0,0), matVP(1,0), matVP(2,0), matVP(3,0));
	D3DXVECTOR4 col1(matVP(0,1), matVP(1,1), matVP(2,1), matVP(3,1));
	D3DXVECTOR4 col2(matVP(0,2), matVP(1,2), matVP(2,2), matVP(3,2));
	D3DXVECTOR4 col3(matVP(0,3), matVP(1,3), matVP(2,3), matVP(3,3));

	// Planes face inward.
	mFrustumPlanes[0] = (D3DXPLANE)(col2);        // near
	mFrustumPlanes[1] = (D3DXPLANE)(col3 - col2); // far
	mFrustumPlanes[2] = (D3DXPLANE)(col3 + col0); // left
	mFrustumPlanes[3] = (D3DXPLANE)(col3 - col0); // right
	mFrustumPlanes[4] = (D3DXPLANE)(col3 - col1); // top
	mFrustumPlanes[5] = (D3DXPLANE)(col3 + col1); // bottom

	for(int i = 0; i < 6; i++)
		D3DXPlaneNormalize(&mFrustumPlanes[i], &mFrustumPlanes[i]);

}

bool Camera::IsVisible(AABBox boundingBox)
{
	for(int i = 0; i < 6; ++i){
		Vector3 vertP = boundingBox.mMin;
		Vector3 vertN = boundingBox.mMax;

		if(mFrustumPlanes[i].a > 0)
			vertP.x = boundingBox.mMax.x;
		if(mFrustumPlanes[i].b > 0)
			vertP.y = boundingBox.mMax.y;
		if(mFrustumPlanes[i].c > 0)
			vertP.z = boundingBox.mMax.z;
		
		if(mFrustumPlanes[i].a > 0)
			vertN.x = boundingBox.mMin.x;
		if(mFrustumPlanes[i].b > 0)
			vertN.y = boundingBox.mMin.y;
		if(mFrustumPlanes[i].c > 0)
			vertN.z = boundingBox.mMin.z;

		if(vertP.Dot(Vector3(mFrustumPlanes[i].a, mFrustumPlanes[i].b, mFrustumPlanes[i].c)) + mFrustumPlanes[i].d < 0)
			return false;
	}

	return true;
}

void Camera::recalculateCameraMatrix()
{
	recalculateViewMatrix();
	recalculateProjMatrix();
}

void Camera::GetScreenRay(const Vector2& screenPos, Vector3* rayPos, Vector3* rayDir)
 {
	_Assert(NULL != rayPos);
	_Assert(NULL != rayDir);

	Vector2 screenLocation;
	gEngine->GetDriver()->GetScreenLocation(screenPos, &screenLocation);

	Vector3& rp = *rayPos;
	rp = mWorldPos;

	Vector3 vp = rp + mWorldForward.Normalized();

	float screenWorldHeight = 2 * tan(mFOV / 2.0f);
	float screenWorldWidth = mAspect * screenWorldHeight;

	vp += (screenLocation.x - 0.5f) * screenWorldWidth * mWorldRight.Normalized();
	vp += -(screenLocation.y - 0.5f) * screenWorldHeight * mWorldUp.Normalized();

	*rayDir = vp - rp;
}	

void Camera::GetCameraParams(float* nearZ, float* farZ, float* fov, float* aspect)
{
	if(nearZ)
		*nearZ = mNearZ;

	if(farZ)
		*farZ = mFarZ;

	if(fov)
		*fov = mFOV;

	if(aspect)
		*aspect = mAspect;
}

void Camera::SetCameraController(CameraController* controller)
{
	SAFE_DELETE(mController);

	mController = controller;
}

void Camera::ApplyCameraController()
{
	_Assert(NULL != mController);
	mController->Apply(this, gEngine->GetFrameTimer()->GetDeltaTime());
}

void Camera::FrameUpdate()
{
	if(mIsParametersDirty)
	{
		recalculateProjMatrix();

		SetParametersDirty(false);
	}

	if(mIsTranformDirty)
	{
		recalculateViewMatrix();
		extractFrustumPlanes();

		SetTransformDirty(false);
	}

	// damp
	if(mDampContext.isDamping)
	{
		if(mDampContext.elapsedTime > mDampContext.totalTime)
		{
			mDampContext.Clear();
			SetWorldPosition(mDampContext.targetPos);
		}
		else
		{
			float deltaTime = gEngine->GetFrameTimer()->GetDeltaTime();

			mDampContext.dampVel += deltaTime * mDampContext.dampAcc;

			Vector3 moveVector = deltaTime * mDampContext.dampVel;

			if(moveVector != Vector3::Zero)
			{
				Translate(moveVector);
			}

			mDampContext.elapsedTime += deltaTime;
		}
	}
}

void Camera::SetParametersDirty( bool isDirty )
{
	mIsParametersDirty = isDirty;
}

void Camera::SetTransformDirty( bool isDirty )
{
	mIsTranformDirty = isDirty;
}

void Camera::SetAspect( float aspect )
{
	mAspect = aspect;
	mIsParametersDirty = true;
}

D3DXMATRIX Camera::OrthoProjMatrix( float orthoWidth ) const
{
	D3DXMATRIX matProj;
	D3DXMatrixOrthoLH(&matProj, orthoWidth, orthoWidth / mAspect, mNearZ, mFarZ);
	return matProj;
}

D3DXMATRIX Camera::ProjMatrix() const
{
	return mMatProj;
}

D3DXMATRIX Camera::ViewMatrix() const
{
	return mMatView;
}

void Camera::DampMoveTo(const Vector3& targetPos, float time, float dampFactor)
{
	mDampContext = DampContext(GetWorldPosition(), targetPos, time, dampFactor);
}

void Camera::OnTransformChanged()
{
	SetTransformDirty(true);
}

void Camera::FocusAt(Object* obj)
{
	const float DIST = 4.0f;

	Vector3 targetPos = obj->GetWorldPosition() - DIST * GetWorldForward().Normalized();
	DampMoveTo(targetPos, 0.5f, 0);
}
