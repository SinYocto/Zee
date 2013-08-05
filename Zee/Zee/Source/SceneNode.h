#ifndef SCENE_NODE_H
#define SCENE_NODE_H

#include "Object.h"
#include "Bound.h"

class Camera;

class SceneNode : public Object
{
public:
	enum DISPLAY_MODE
	{
		WIRE_FRAME,
		SOLID
	};

	struct Attribute
	{
		DISPLAY_MODE displayMode;
		bool isStatic;
		bool drawBBox;
	};

public:
	SceneNode(const wchar_t* name, SceneNode* parent = NULL, Vector3 position = Vector3::Zero, 
		Quaternion orient = Quaternion(0, 0, 0))
		:Object(parent, position, orient)
	{
		YString::Copy(mName, _countof(mName), name);

		mAttribute.displayMode = SOLID;
		mAttribute.isStatic = false;
		mAttribute.drawBBox = false;
	}

	~SceneNode()
	{

	}

	void SetID(DWORD id);
	void Detach();

	void EnableBBoxDrawer(bool enalbe);
	void SetDisplayMode(DISPLAY_MODE displayMode);

	AABBox GetAABBox();

	virtual void Draw(Camera* camera)
	{
		for(std::list<Object*>::iterator iter = mChildren.begin(); iter != mChildren.end(); ++iter)
		{
			SceneNode* node = static_cast<SceneNode*>(*iter);
			node->Draw(camera);
		}
	}

protected:
	DWORD mID;
	wchar_t mName[MAX_STR_LEN];

	AABBox mAABBox;

	Attribute mAttribute;
};

#endif