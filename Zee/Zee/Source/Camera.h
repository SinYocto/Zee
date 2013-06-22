#ifndef CAMERA_H
#define CAMERA_H

#include"Object.h"
#include"Math.h"
#include"Bound.h"

class Camera:public Object
{
public:
	Camera(const Vector3 pos = Vector3(0, 0, -200), const Vector3 target = Vector3::Zero, 
		float fov = PI/2, float asp = 1.0f, float nZ = 1.0f, float fZ = 1000.0f)
		:Object(NULL, pos)
		,FOV(fov)
		,aspect(asp)
		,nearZ(nZ)
		,farZ(fZ)
	{
		LookAt(target);
		recalculateCameraMatrix();
		extractFrustumPlanes();

		isTranformDirty = false;
		isParametersDirty = false;
	}

	D3DXMATRIX ViewMatrix() const 
	{ 
		return matView;
	}

	D3DXMATRIX ProjMatrix() const
	{
		return matProj;
	}

	bool IsVisible(BoundingBox boundingBox);


	void SetTransformDirty(bool isDirty)
	{
		isTranformDirty = isDirty;
	}

	void SetParametersDirty(bool isDirty)
	{
		isParametersDirty = isDirty;
	}

	void Update()
	{
		if(isParametersDirty)
		{
			recalculateProjMatrix();

			SetParametersDirty(false);
		}

		if(isTranformDirty)
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
	float FOV;
	float aspect;
	float nearZ;
	float farZ;

	D3DXPLANE frustumPlanes[6];

	D3DXMATRIX matProj;
	D3DXMATRIX matView;

	bool isTranformDirty;
	bool isParametersDirty;
};



#endif