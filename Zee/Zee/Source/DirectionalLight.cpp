#include"DirectionalLight.h"
#include "LightManager.h"

void DirectionalLight::Enable(bool enable)
{
	if(enable == isEnabled)
	{
		return;
	}
	else
	{
		if(enable == true)
		{
			if(LightManager::numActiveDirectionalLights < MAX_NUM_DIRECTIONAL_LIGHTS)
			{
				isEnabled = true;
				LightManager::numActiveDirectionalLights++;

				LightManager::isDirectionalLightsDirty = true;
			}
		}
		else
		{
			isEnabled = false;
			LightManager::numActiveDirectionalLights--;

			LightManager::isDirectionalLightsDirty = true;
		}
	}
}

bool DirectionalLight::IsEnabled()
{
	return isEnabled;
}

void DirectionalLight::SetValue(D3DXCOLOR _color, Vector3 _dir, float _intensity)
{
	color = _color;
	direction = _dir;
	intensity = _intensity;

	LightManager::isDirectionalLightsDirty = true;
}