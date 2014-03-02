#include "DirectionalLight.h"
#include "SceneNode.h"

class Billboard;

class DirectionalLightNode : public SceneNode
{
public:
	DirectionalLightNode(SceneNode* parent, DirectionalLight* dirLight);
	~DirectionalLightNode();

	Billboard* GetBillboard();
	DirectionalLight* GetDirLight();

	void SetLightDirection(const Vector3& dir);
	void SetLightIntensity(float intensity);
	void SetLightColor(D3DXCOLOR color);

	bool IsLightEnabled();

	virtual void OnTransformChanged();

private:
	virtual void updateAABBoxSelf();

private:
	DirectionalLight* mDirLight;
	Billboard* mBillboard;
};