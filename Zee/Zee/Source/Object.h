#ifndef OBJECT_H
#define OBJECT_H

#include "Utility.h"
#include "D3DUtility.h"
#include"Math.h"
#include <list>
#include <algorithm>

// ע������3������ϵ: object space(��������ϵ), local space(����������ϵ), world space
// ���⻹��ע��, ��RotateLocal, LocalVectorToWorld, LocalToWorldMatrix�������local������ܻ���������,
// ��ʵ��ָ����object space, ����RotateLocal����ָ��������������ϵ��(object space)��תһ���Ƕ�, ���Ǹ����������ϵ(local space)

// #Object���ݳ�Ա����
// Ҫ������������, ������������λ�ú;���λ�õ�ĳһ���ı�Ҫ������һ��, ��θ�����λ�øı�Ҫ�����Ӷ���λ��

// #��������Ӷ���ɾ��ʱ�Ĵ���
// ��Ҫ���¸�����ָ��, �Ӷ����б�, ��Ҫdetach��ȡ�����λ�õ�������ϵ

class Object
{
public:
	Object(Object* parent = NULL, Vector3 position = Vector3::Zero, Quaternion orient = Quaternion(0, 0, 0))
		:mParent(parent)
		,mRelativePos(position)
		,mRelativeOrient(orient)
		,mRelativeScale(Vector3(1.0f, 1.0f, 1.0f))
	{
		updateWorldPosition();
		updateWorldOrientation();
		updateWorldScale();
		updateLocalAxis();

		if(NULL != mParent)
		{
			mParent->AddChild(this);
		}
	}

	~Object()
	{
		Detach();

		for(std::list<Object*>::iterator iter = mChildren.begin(); iter != mChildren.end();)
		{
			// û��ֱ�ӵ���children��Detach����, ��Ϊ�Ǳ߱�����ɾ��, ��Ҫ��¼ɾ������¸�������λ��
			(*iter)->mRelativePos = (*iter)->mWorldPos;
			(*iter)->mRelativeOrient = (*iter)->mWorldOrient;
			(*iter)->mRelativeScale = (*iter)->mWorldScale;
			(*iter)->mParent = NULL;

			iter = mChildren.erase(iter);
		}
	}

	void AddChild(Object* child);
	void RemoveChild(Object* child);

	void SetParent(Object* parent);
	void Detach();

	void LookAt(const Vector3& lookAtPos);
	void Rotate(float eulerX, float eulerY, float eulerZ);
	void RotateLocal(float eulerX, float eulerY, float eulerZ);

	void Translate(const Vector3& moveVector);
	void Translate(float x, float y, float z);
	void TranslateLocal(const Vector3& moveVector);
	void TranslateLocal(float x, float y, float z);

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

	void SetRelativeOrientation(float eulerX, float eulerY, float eulerZ);
	void SetWorldOrientation(float eulerX, float eulerY, float eulerZ);

private:
	void updateWorldPosition();
	void updateWorldOrientation();
	void updateWorldScale();

	void updateRelativePostion();
	void updateRelativeOrientation();
	void updateRelativeScale();

	void updateLocalAxis();

	void updateChildrenTransform();		// ������λ�仯��, �����Ӷ���λ

	void rotate(Quaternion worldRotation);
	void rotateLocal(Quaternion localRotation);

protected:
	Object* mParent;
	std::list<Object*> mChildren;

	Vector3 mRelativePos;
	Quaternion mRelativeOrient;
	Vector3 mRelativeScale;

	Vector3 mWorldPos;
	Quaternion mWorldOrient;
	Vector3 mWorldScale;

	Vector3 mWorldRight;
	Vector3 mWorldUp;
	Vector3 mWorldForward;
};


#endif