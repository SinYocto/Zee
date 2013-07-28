#ifndef CAMERA_H
#define CAMERA_H

#include"Object.h"
#include"Math.h"
#include"Bound.h"

class Camera:public Object
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

	bool IsVisible(BoundingBox boundingBox);


	void SetTransformDirty(bool isDirty)
	{
		mIsTranformDirty = isDirty;
	}

	void SetParametersDirty(bool isDirty)
	{
		mIsParametersDirty = isDirty;
	}

	void Update()
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