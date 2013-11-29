#ifndef BILLBOARDNODE_H
#define BILLBOARDNODE_H

#include "SceneNode.h"
#include "Billboard.h"

#include "ResourceMgr.h"

class BillboardNode : public SceneNode
{
public:
	BillboardNode(const wchar_t* name, float width, float height, D3DCOLOR color)
		:SceneNode(name)
	{
		mType = SCENE_NODE_BILLBOARD;
		mBillboard = new Billboard(width, height, color);

		ResourceMgr::AddBillboard(mBillboard);
	}

	~BillboardNode()
	{
		SAFE_DROP(mBillboard);
	}

	virtual void Draw(Camera* camera);

	Billboard* GetBillboard();

private:
	virtual void updateAABBox();

private:
	Billboard* mBillboard;
};

#endif