#include "DirectionalLight.h"
#include "SceneNode.h"

class Billboard;

class DirectionalLightNode : public SceneNode
{
public:
	DirectionalLightNode(SceneNode* parent, DirectionalLight* dirLight);
	~DirectionalLightNode();

	Billboard* GetBillboard();

	virtual void OnTransformChanged();

private:
	virtual void updateAABBox();

private:
	DirectionalLight* mDirLight;
	Billboard* mBillboard;
};