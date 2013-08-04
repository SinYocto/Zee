#include "AmbientLight.h"
#include "DirectionalLight.h"
#include "PointLight.h"

class LightManager
{
public:
	static void AddDirectionalLight(DirectionalLight* light);
	static void AddPointLight(PointLight* light);

	static void SetAmbientLight(D3DXCOLOR _color, float _intensity);
	static D3DXCOLOR GetFinalAmbientColor();

	static void GetDirLight(const wchar_t* name, DirectionalLight** dirLight);
	static void GetPointLight(const wchar_t* name, PointLight** pointLight);

	static void Update();

public:
	static int numActiveDirectionalLights;
	static int numActivePointLights;

	static bool isDirectionalLightsDirty;
	static bool isPointLightsDirty;

	static DirectionalLightData directionalLightsData[MAX_NUM_DIRECTIONAL_LIGHTS];
	static PointLightData pointLightsData[MAX_NUM_POINT_LIGHTS];

private:
	static AmbientLight mAmbientLight;

	static std::list<DirectionalLight*> mDirlLights;
	static std::list<PointLight*> mPointLights;

	static DWORD curDirLightID;
	static DWORD curPointLightID;
};