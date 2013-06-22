#include"PointLight.h"
#include "LightManager.h"

void PointLight::Enable(bool enable)
{
	if(enable == isEnabled)
	{
		return;
	}
	else
	{
		if(enable == true)
		{
			if(LightManager::numActivePointLights < MAX_NUM_POINT_LIGHTS)
			{
				isEnabled = true;
				LightManager::numActivePointLights++;

				LightManager::isPointLightsDirty = true;
			}
		}
		else
		{
			isEnabled = false;
			LightManager::numActivePointLights--;

			LightManager::isPointLightsDirty = true;
		}
	}
}

bool PointLight::IsEnabled()
{
	return isEnabled;
}

void PointLight::SetValue(D3DXCOLOR _color, Vector3 _pos, Vector3 _attenuation, float _intensity)
{
	color = _color;
	position = _pos;
	attenuation = _attenuation;
	intensity = _intensity;

	LightManager::isPointLightsDirty = true;
}