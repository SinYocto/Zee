#include "SceneNode.h"
#include "DebugDrawer.h"
#include "Engine.h"

void SceneNode::SetID(DWORD id)
{
	mID = id;
}

void SceneNode::Detach()
{
	Object::Detach();
	SetParent(gEngine->GetSceneManager()->GetRoot());
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
		// QUESTION:会不会因此反复重复设置渲染状态引起渲染效率降低?
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
	updateAABBox();		// TODO:当前没管sceneNode的isStatic标志, 通通都更新计算AABB

	for(std::list<Object*>::iterator iter = mChildren.begin(); iter != mChildren.end(); ++iter)
	{
		SceneNode* node = static_cast<SceneNode*>(*iter);
		node->FrameUpdate();
	}
}

void SceneNode::updateAABBox()
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

SceneNode::DISPLAY_MODE SceneNode::GetDisplayMode()
{
	return mAttribute.displayMode;
}

bool SceneNode::GetDrawBBoxFlag()
{
	return mAttribute.drawBBox;
}

