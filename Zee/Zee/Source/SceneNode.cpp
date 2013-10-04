#include "SceneNode.h"
#include "SceneManager.h"
#include "DebugDrawer.h"

void SceneNode::SetID(DWORD id)
{
	mID = id;
}

void SceneNode::Detach()
{
	Object::Detach();
	SetParent(SceneManager::root);
}

void SceneNode::SetDrawBBoxFlag(bool drawBBox)
{
	mAttribute.drawBBox = drawBBox;
}

AABBox SceneNode::GetAABBox()
{
	return mAABBox;
}

void SceneNode::SetDisplayMode(DISPLAY_MODE displayMode)
{
	mAttribute.displayMode = displayMode;

	for(std::list<Object*>::iterator iter = mChildren.begin(); iter != mChildren.end(); ++iter)
	{
		// QUESTION:�᲻����˷����ظ�������Ⱦ״̬������ȾЧ�ʽ���?
		SceneNode* node = static_cast<SceneNode*>(*iter);
		node->SetDisplayMode(displayMode);
	}
}

void SceneNode::SetStaticFlag(bool isStatic)
{
	mAttribute.isStatic = isStatic;
}

void SceneNode::FrameUpdate()
{
	calcCurrentAABBox();		// TODO:��ǰû��sceneNode��isStatic��־, ͨͨ�����¼���AABB

	for(std::list<Object*>::iterator iter = mChildren.begin(); iter != mChildren.end(); ++iter)
	{
		SceneNode* node = static_cast<SceneNode*>(*iter);
		node->FrameUpdate();
	}
}

void SceneNode::calcCurrentAABBox()
{
	mAABBox = AABBox::Invalid;

	for(std::list<Object*>::iterator iter = mChildren.begin(); iter != mChildren.end(); ++iter)
	{
		SceneNode* node = static_cast<SceneNode*>(*iter);
		mAABBox = AABBox::CombineBBox(mAABBox, node->GetAABBox());
	}
}

void SceneNode::Draw(Camera* camera)
{
	if(mAttribute.drawBBox && mAABBox.isValid())
	{
		DebugDrawer::DrawAABBox(mAABBox, 0xffff0000, camera);
	}
}

SceneNode* SceneNode::RayIntersect(const Vector3& rayPos, const Vector3& rayDir, Vector3* hitPos, float* dist)
{
	Vector3 hitP;
	float d;

	if(IntersectRayAABB(rayPos, rayDir, GetAABBox(), &hitP, &d))
	{
		if(mChildren.size() == 0)
		{
			if(hitPos)
				*hitPos = hitP;

			if(dist)
				*dist = d;

			return this;
		}
		else
		{
			d = FLT_MAX;
			SceneNode* nextNode = NULL;
			for(std::list<Object*>::iterator iter = mChildren.begin(); iter != mChildren.end(); ++iter)
			{
				SceneNode* curNode = static_cast<SceneNode*>(*iter);

				float tempD = 0;
				if(IntersectRayAABB(rayPos, rayDir, curNode->GetAABBox(), NULL, &tempD) && tempD < d)
				{
					d = tempD;
					nextNode = curNode;
				}
			}

			if(nextNode)
				return nextNode->RayIntersect(rayPos, rayDir, hitPos, dist);
			else
				return NULL;
		}	
	}
	else
	{
		return NULL;
	}
}

SceneNode::NODE_TYPE SceneNode::GetNodeType()
{
	return mType;
}

