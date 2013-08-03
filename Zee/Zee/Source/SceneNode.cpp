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

