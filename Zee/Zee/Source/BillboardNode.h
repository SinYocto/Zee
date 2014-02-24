#ifndef BILLBOARDNODE_H
#define BILLBOARDNODE_H

#include "SceneNode.h"
#include "Billboard.h"

class BillboardNode : public SceneNode
{
public:
	BillboardNode(const wchar_t* name, float width, float height, D3DCOLOR color);

	~BillboardNode()
	{
		SAFE_DROP(mBillboard);
	}

	virtual void Draw(Camera* camera);

	Billboard* GetBillboard();

private:
	virtual void updateAABBoxSelf();

private:
	Billboard* mBillboard;
};

#endif