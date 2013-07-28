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

void DirectionalLight::SetValue(D3DXCOLOR color, Vector3 dir, float intensity)
{
	mColor = color;
	mDirection = dir;
	mIntensity = intensity;

	LightManager::isDirectionalLightsDirty = true;
}