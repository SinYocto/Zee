#ifndef OBJECT_H
#define OBJECT_H

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
	Object(Object* _parent = NULL, Vector3 position = Vector3(0,0,0), Quaternion rotation = Quaternion(0, 0, 0))
		:parent(_parent)
		,relativePosition(position)
		,relativeOrientation(rotation)
		,relativeScale(Vector3(1.0f, 1.0f, 1.0f))
	{
		updateWorldPosition();
		updateWorldOrientation();
		updateWorldScale();
		updateLocalAxis();

		if(NULL != parent)
		{
			parent->AddChild(this);
		}
	}

	~Object()
	{
		Detach();

		for(std::list<Object*>::iterator iter = children.begin(); iter != children.end();)
		{
			// û��ֱ�ӵ���children��Detach����, ��Ϊ�Ǳ߱�����ɾ��, ��Ҫ��¼ɾ������¸�������λ��
			(*iter)->relativePosition = (*iter)->worldPosition;
			(*iter)->relativeOrientation = (*iter)->worldOrientation;
			(*iter)->relativeScale = (*iter)->worldScale;
			(*iter)->parent = NULL;

			iter = children.erase(iter);
		}
	}

	void AddChild(Object* child)
	{
		_Assert(NULL != child);

		if(std::find(children.begin(), children.end(), child) != children.end())
		{
			// children���Ѵ��ڴ˶���ָ��
			return;
		}
		else
		{
			children.push_back(child);

			child->parent = this;
			child->updateRelativePostion();
			child->updateRelativeOrientation();
			child->updateRelativeScale();
		}
	}

	void SetParent(Object* _parent)
	{
		if(NULL == _parent)
		{
			Detach();
		}
		else
		{
			_parent->AddChild(this);
		}
	}

	void RemoveChild(Object* child)
	{
		_Assert(NULL != child);
		_Assert(std::find(children.begin(), children.end(), child) != children.end());

		if(std::find(children.begin(), children.end(), child) == children.end())
		{
			return;
		}
		else 
		{			
			child->Detach();
		}
	}

	void Detach()
	{
		if(NULL == parent)
		{
			return;
		}
		else
		{
			std::list<Object*>::iterator iter = std::find(parent->children.begin(), parent->children.end(), this);
			_Assert(iter != parent->children.end());

			iter = parent->children.erase(iter);
			parent = NULL;

			relativePosition = worldPosition;
			relativeOrientation = worldOrientation;
			relativeScale = worldScale;
		}
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
	void updateRelativeScale();

	void updateLocalAxis();

	void updateChildrenLocation();		// ������λ�仯��, �����Ӷ���λ

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