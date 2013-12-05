#include "PointLight.h"
#include "SceneNode.h"

class Billboard;

class PointLightNode : public SceneNode
{
public:
	PointLightNode(SceneNode* parent, PointLight* pointLight);
	~PointLightNode();

	Billboard* GetBillboard();
	PointLight* GetPointLight();

	void SetLightPosition(const Vector3& pos);
	void SetLightIntensity(float intensity);
	void SetLightColor(D3DXCOLOR color);

	virtual void OnTransformChanged();

private:
	virtual void updateAABBox();

private:
	PointLight* mPointLight;
	Billboard* mBillboard;
};