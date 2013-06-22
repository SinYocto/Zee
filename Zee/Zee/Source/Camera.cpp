#include"Camera.h"

void Camera::recalculateViewMatrix()
{
	D3DXMATRIX matRotTranspose, matTransInverse;
	D3DXMatrixTranspose(&matRotTranspose, &(relativeOrientation.Matrix()));
	D3DXMatrixTranslation(&matTransInverse, -relativePosition.x, -relativePosition.y, -relativePosition.z);
	matView = matTransInverse * matRotTranspose; 
}

void Camera::recalculateProjMatrix()
{
	D3DXMatrixPerspectiveFovLH(&matProj, FOV, aspect, nearZ, farZ);
}

void Camera::extractFrustumPlanes()
{
	D3DXMATRIX matVP = ViewMatrix() * ProjMatrix();

	D3DXVECTOR4 col0(matVP(0,0), matVP(1,0), matVP(2,0), matVP(3,0));
	D3DXVECTOR4 col1(matVP(0,1), matVP(1,1), matVP(2,1), matVP(3,1));
	D3DXVECTOR4 col2(matVP(0,2), matVP(1,2), matVP(2,2), matVP(3,2));
	D3DXVECTOR4 col3(matVP(0,3), matVP(1,3), matVP(2,3), matVP(3,3));

	// Planes face inward.
	frustumPlanes[0] = (D3DXPLANE)(col2);        // near
	frustumPlanes[1] = (D3DXPLANE)(col3 - col2); // far
	frustumPlanes[2] = (D3DXPLANE)(col3 + col0); // left
	frustumPlanes[3] = (D3DXPLANE)(col3 - col0); // right
	frustumPlanes[4] = (D3DXPLANE)(col3 - col1); // top
	frustumPlanes[5] = (D3DXPLANE)(col3 + col1); // bottom

	for(int i = 0; i < 6; i++)
		D3DXPlaneNormalize(&frustumPlanes[i], &frustumPlanes[i]);

}

bool Camera::IsVisible(BoundingBox boundingBox)
{
	/* 以下方法会在invisible时误判为visible。一种解决方法是再加上viewFrustum8个顶点是否在boundingBox内的判断。
	for(int i = 0; i < 6; ++i){

		bool isBehind = true;
		for(int j = 0; j < 8; ++j){
			if(boundingBox.GetVertex(j).Dot(Vector3(frustumPlanes[i].a, frustumPlanes[i].b, frustumPlanes[i].c)) + frustumPlanes[i].d > 0)
				isBehind = false;
		}

		if(isBehind)
			return false;
	}

	return true;*/

	for(int i = 0; i < 6; ++i){
		Vector3 vertP = boundingBox.min;
		Vector3 vertN = boundingBox.max;

		if(frustumPlanes[i].a > 0)
			vertP.x = boundingBox.max.x;
		if(frustumPlanes[i].b > 0)
			vertP.y = boundingBox.max.y;
		if(frustumPlanes[i].c > 0)
			vertP.z = boundingBox.max.z;
		
		if(frustumPlanes[i].a > 0)
			vertN.x = boundingBox.min.x;
		if(frustumPlanes[i].b > 0)
			vertN.y = boundingBox.min.y;
		if(frustumPlanes[i].c > 0)
			vertN.z = boundingBox.min.z;

		if(vertP.Dot(Vector3(frustumPlanes[i].a, frustumPlanes[i].b, frustumPlanes[i].c)) + frustumPlanes[i].d < 0)
			return false;
	}

	return true;

}

void Camera::recalculateCameraMatrix()
{
	recalculateViewMatrix();
	recalculateProjMatrix();
}
