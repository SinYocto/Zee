#ifndef OBJECT_H
#define OBJECT_H

#include "Common.h"
#include "Math.h"

class Object
{
public:
	Object(Object* parent = NULL, Vector3 position = Vector3::Zero, Quaternion orient = Quaternion(0, 0, 0))
		:mParent(parent)
		,mRelativePos(position)
		,mRelativeOrient(orient)
		,mScale(Vector3(1.0f, 1.0f, 1.0f))
	{
		updateWorldPosition();
		updateWorldOrientation();
		updateLocalAxis();

		if(NULL != mParent)
		{
			mParent->AddChild(this);
		}
	}

	virtual ~Object()
	{
		Detach();

		for(std::list<Object*>::iterator iter = mChildren.begin(); iter != mChildren.end();)
		{
			Object* object = *iter++;
			SAFE_DELETE(object);
		}
	}

	void AddChild(Object* child);
	void RemoveChild (Object* child);

	void SetParent(Object* parent);
	void Detach();

	void LookAt(const Vector3& lookAtPos);
	void Rotate(float eulerX, float eulerY, float eulerZ);
	void RotateLocal(float eulerX, float eulerY, float eulerZ);

	void Translate(const Vector3& moveVector);
	void Translate(float x, float y, float z);
	void TranslateLocal(const Vector3& moveVector);
	void TranslateLocal(float x, float y, float z);

	void SetScale(const Vector3& scale);
	void Scale(const Vector3& scale);

	Vector3 LocalVectorToWorld(const Vector3& localVec);
	Vector3 WorldVectorToLocal(const Vector3& worldVec);

	Quaternion LocalRotationToWorld(const Quaternion& localRotation);
	Quaternion WorldRotationToLocal(const Quaternion& worldRotation);

	D3DXMATRIX LocalToWorldMatrix();

	void SetRelativePosition(const Vector3& pos);
	void SetRelativePosition(float x, float y, float z);
	Vector3 GetRelativePosition();

	void SetWorldPosition(const Vector3& pos);
	void SetWorldPosition(float x, float y, float z);
	Vector3 GetWorldPosition();

	Vector3 GetWorldPosition(const Vector3& localMoveVector);

	Quaternion GetWorldOrient();

	Vector3 GetWorldRight();
	Vector3 GetWorldUp();
	Vector3 GetWorldForward();

	void SetRelativeOrientation(float eulerX, float eulerY, float eulerZ);
	void SetWorldOrientation(Quaternion orient);
	void SetWorldOrientation(float eulerX, float eulerY, float eulerZ);

private:
	void updateWorldPosition();
	void updateWorldOrientation();

	void updateRelativePostion();
	void updateRelativeOrientation();

	void updateLocalAxis();

	void updateChildrenTransform();		// 父对象方位变化后, 更新子对象方位
	void scaleChildren(const Vector3& scale);

	void rotate(Quaternion worldRotation);
	void rotateLocal(Quaternion localRotation);

protected:
	Object* mParent;
	std::list<Object*> mChildren;

	Vector3 mRelativePos;
	Quaternion mRelativeOrient;

	Vector3 mWorldPos;
	Quaternion mWorldOrient;

	Vector3 mWorldRight;
	Vector3 mWorldUp;
	Vector3 mWorldForward;

	Vector3 mScale;
};


#endif