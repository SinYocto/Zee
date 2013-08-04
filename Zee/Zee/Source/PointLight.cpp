#include"PointLight.h"
#include "LightManager.h"

void PointLight::Enable(bool enable)
{
	if(enable == mIsEnabled)
	{
		return;
	}
	else
	{
		if(enable == true)
		{
			if(LightManager::numActivePointLights < MAX_NUM_POINT_LIGHTS)
			{
				mIsEnabled = true;
				LightManager::numActivePointLights++;

				LightManager::isPointLightsDirty = true;
			}
		}
		else
		{
			mIsEnabled = false;
			LightManager::numActivePointLights--;

			LightManager::isPointLightsDirty = true;
		}
	}
}

void PointLight::SetID(DWORD id)
{
	mID = id;
}

bool PointLight::IsEnabled()
{
	return mIsEnabled;
}

void PointLight::SetValue(D3DXCOLOR color, Vector3 pos, Vector3 attenuation, float intensity)
{
	mColor = color;
	mPosition = pos;
	mAttenuation = attenuation;
	mIntensity = intensity;

	LightManager::isPointLightsDirty = true;
}

wchar_t* PointLight::GetName()
{
	return mName;
}