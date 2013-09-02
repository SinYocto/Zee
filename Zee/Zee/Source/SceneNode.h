#ifndef SCENE_NODE_H
#define SCENE_NODE_H

#include "Object.h"
#include "Bound.h"

class Camera;

class SceneNode : public Object
{
public:
	enum NODE_TYPE
	{
		SCENE_NODE_NULL,
		SCENE_NODE_MESH,
		SCENE_NODE_MODEL,
		SCENE_NODE_BILLBOARD
	};

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
		,mType(SCENE_NODE_NULL)
	{
		YString::Copy(mName, _countof(mName), name);

		mAttribute.displayMode = SOLID;
		mAttribute.isStatic = true;
		mAttribute.drawBBox = false;
	}

	~SceneNode()
	{

	}

	void SetID(DWORD id);
	void Detach();

	void SetDrawBBoxFlag(bool drawBBox);
	void SetStaticFlag(bool isStatic);
	void SetDisplayMode(DISPLAY_MODE displayMode);

	AABBox GetAABBox();

	SceneNode* RayIntersect(const Vector3& rayPos, const Vector3& rayDir, Vector3* hitPos, float* dist);

	void FrameUpdate();

	virtual void Draw(Camera* camera);

	void DrawAll(Camera* camera)
	{
		Draw(camera);

		for(std::list<Object*>::iterator iter = mChildren.begin(); iter != mChildren.end(); ++iter)
		{
			SceneNode* node = static_cast<SceneNode*>(*iter);
			node->DrawAll(camera);
		}
	}

	NODE_TYPE GetNodeType();

protected:
	virtual void calCurrentAABBox();

protected:
	DWORD mID;
	wchar_t mName[MAX_STR_LEN];

	AABBox mAABBox;

	Attribute mAttribute;

	NODE_TYPE mType;
};

#endif