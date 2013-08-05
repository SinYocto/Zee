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

void SceneNode::EnableBBoxDrawer(bool enalbe)
{
	mAttribute.drawBBox = enalbe;
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

