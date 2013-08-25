#ifndef CAMERA_H
#define CAMERA_H

#include "Object.h"
#include "Bound.h"

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
	{
		LookAt(target);
		recalculateCameraMatrix();
		extractFrustumPlanes();

		mIsTranformDirty = false;
		mIsParametersDirty = false;
	}

	D3DXMATRIX ViewMatrix() const 
	{ 
		return mMatView;
	}

	D3DXMATRIX ProjMatrix() const
	{
		return mMatProj;
	}

	D3DXMATRIX OrthoProjMatrix(float orthoWidth) const
	{
		D3DXMATRIX matProj;
		D3DXMatrixOrthoLH(&matProj, orthoWidth, orthoWidth / mAspect, mNearZ, mFarZ);
		return matProj;
	}

	bool IsVisible(AABBox boundingBox);

	void SetAspect(float aspect)
	{
		mAspect = aspect;
		mIsParametersDirty = true;
	}

	void SetTransformDirty(bool isDirty)
	{
		mIsTranformDirty = isDirty;
	}

	void SetParametersDirty(bool isDirty)
	{
		mIsParametersDirty = isDirty;
	}

	void FrameUpdate()
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
	}

	void GetScreenRay(const Vector2& screenPos, Vector3* rayPos, Vector3* rayDir);

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
};



#endif