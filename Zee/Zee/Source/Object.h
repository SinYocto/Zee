#ifndef OBJECT_H
#define OBJECT_H

#include "Utility.h"
#include "D3DUtility.h"
#include"Math.h"
#include <list>
#include <algorithm>

// 注意区分3个坐标系: object space(自身坐标系), local space(父对象坐标系), world space
// 另外还需注意, 像RotateLocal, LocalVectorToWorld, LocalToWorldMatrix这里面的local含义可能会引发歧义,
// 其实际指的是object space, 比如RotateLocal方法指的是在自身坐标系下(object space)旋转一定角度, 而非父对象的坐标系(local space)

// #Object数据成员更新
// 要考虑两个问题, 首先自身的相对位置和绝对位置的某一个改变要更新另一个, 其次父对象位置改变要更新子对象位置

// #父对象或子对象删除时的处理
// 需要更新父对象指针, 子对象列表, 需要detach来取消相对位置的依附关系

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
			// 没有直接调用children的Detach方法, 因为是边遍历边删除, 需要记录删除后的下个迭代器位置
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

	void updateChildrenTransform();		// 父对象方位变化后, 更新子对象方位

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