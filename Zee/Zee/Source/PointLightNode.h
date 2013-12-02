#include "PointLight.h"
#include "SceneNode.h"

class Billboard;

class PointLightNode : public SceneNode
{
public:
	PointLightNode(SceneNode* parent, PointLight* pointLight);
	~PointLightNode();

	Billboard* GetBillboard();

	virtual void OnTransformChanged();

private:
	virtual void updateAABBox();

private:
	PointLight* mPointLight;
	Billboard* mBillboard;
};