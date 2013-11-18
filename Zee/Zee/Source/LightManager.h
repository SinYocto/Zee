#ifndef LIGHT_MANAGER_H
#define LIGHT_MANAGER_H

#include "AmbientLight.h"
#include "DirectionalLight.h"
#include "PointLight.h"

class LightManager
{
public:
	LightManager();
	~LightManager();

	void Init();
	void Destroy();

	void FrameUpdate();

	void AddDirectionalLight(DirectionalLight* light);
	void AddPointLight(PointLight* light);

	void SetAmbientLight(D3DXCOLOR _color, float _intensity);
	D3DXCOLOR GetFinalAmbientColor();

	void GetDirLight(const wchar_t* name, DirectionalLight** dirLight);
	void GetPointLight(const wchar_t* name, PointLight** pointLight);

	DirectionalLightData* GetDirectionalLightsData();
	PointLightData* GetPointLightsData();

	int GetActiveDirectionalLightCounts();
	int GetActivePointLightCounts();

	void SetDirectionalLightDirtyFlag(bool isDirty);
	void SetPointLightDirtyFlag(bool isDirty);

	void EnableOneDirectionalLight(bool enable);
	void EnableOnePointLight(bool enable);

private:
	AmbientLight* mAmbientLight;

	std::list<DirectionalLight*> mDirlLights;
	std::list<PointLight*> mPointLights;

	DWORD curDirLightID;
	DWORD curPointLightID;

	int numActiveDirectionalLights;
	int numActivePointLights;

	bool isDirectionalLightsDirty;
	bool isPointLightsDirty;

	DirectionalLightData directionalLightsData[MAX_NUM_DIRECTIONAL_LIGHTS];
	PointLightData pointLightsData[MAX_NUM_POINT_LIGHTS];
};

#endif