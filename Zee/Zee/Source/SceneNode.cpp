#include "SceneNode.h"
#include "SceneManager.h"

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
	calCurrentAABBox();		// TODO:当前没管sceneNode的isStatic标志, 通通都更新计算AABB

	for(std::list<Object*>::iterator iter = mChildren.begin(); iter != mChildren.end(); ++iter)
	{
		SceneNode* node = static_cast<SceneNode*>(*iter);
		node->FrameUpdate();
	}
}

