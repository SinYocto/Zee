#ifndef OBJECT_H
#define OBJECT_H

#include "D3DUtility.h"
#include"Math.h"
#include <list>

// ע������3������ϵ: object space(��������ϵ), local space(����������ϵ), world space
// ���⻹��ע��, ��RotateLocal, LocalVectorToWorld, LocalToWorldMatrix�������local������ܻ���������,
// ��ʵ��ָ����object space, ����RotateLocal����ָ��������������ϵ��(object space)��תһ���Ƕ�, ���Ǹ����������ϵ(local space)

class Object
{
public:
	Object(Object* _parent = NULL, Vector3 position = Vector3(0,0,0), Quaternion rotation = Quaternion(0, 0, 0))
		:parent(_parent)
		,relativePosition(position)
		,relativeOrientation(rotation)
		,relativeScale(Vector3(1.0f, 1.0f, 1.0f))
	{
		if(NULL != parent)
			parent->AddChild(this);

		updateWorldPosition();
		updateWorldOrientation();
		updateWorldScale();

		updateLocalAxis();
	}

	void AddChild(Object* child)
	{
		_Assert(NULL != child);
		children.push_back(child);
	}

	void LookAt(Vector3 lookAtPos);
	void Rotate(float eulerX, float eulerY, float eulerZ);
	void RotateLocal(float eulerX, float eulerY, float eulerZ);

	void Translate(Vector3 moveVector);
	void Translate(float x, float y, float z);
	void TranslateLocal(Vector3 moveVector);
	void TranslateLocal(float x, float y, float z);

	Vector3 LocalVectorToWorld(Vector3 localVec);
	Vector3 WorldVectorToLocal(Vector3 worldVec);

	Quaternion LocalRotationToWorld(Quaternion localRotation);
	Quaternion WorldRotationToLocal(Quaternion worldRotation);

	D3DXMATRIX LocalToWorldMatrix();

	void SetRelativePosition(Vector3 pos)
	{
		relativePosition = pos;
		updateWorldPosition();
	}

	void SetRelativePosition(float x, float y, float z)
	{
		SetRelativePosition(Vector3(x, y, z));
	}

	Vector3 GetRelativePosition()
	{
		return relativePosition;
	}

	void SetWorldPosition(Vector3 pos)
	{
		worldPosition = pos;
		updateRelativePostion();
	}

	void SetWorldPosition(float x, float y, float z)
	{
		SetWorldPosition(Vector3(x, y, z));
	}

	void SetRelativeOrientation(float eulerX, float eulerY, float eulerZ)
	{
		relativeOrientation = Quaternion(eulerX, eulerY, eulerZ);
		updateWorldOrientation();
		updateLocalAxis();
	}

	void SetWorldOrientation(float eulerX, float eulerY, float eulerZ)
	{
		worldOrientation = Quaternion(eulerX, eulerY, eulerZ);
		updateRelativeOrientation();
		updateLocalAxis();
	}


private:
	void updateWorldPosition();
	void updateWorldOrientation();
	void updateWorldScale();

	void updateRelativePostion();
	void updateRelativeOrientation();

	void updateLocalAxis();

	void rotate(Quaternion worldRotation);
	void rotateLocal(Quaternion localRotation);

protected:
	Object* parent;
	std::list<Object*> children;

	Vector3 relativePosition;
	Quaternion relativeOrientation;
	Vector3 relativeScale;

	Vector3 worldPosition;
	Quaternion worldOrientation;
	Vector3 worldScale;

	Vector3 worldRightVector;
	Vector3 worldUpVector;
	Vector3 worldForwardVector;
};


#endif