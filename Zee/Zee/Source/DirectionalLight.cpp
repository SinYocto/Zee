#include"DirectionalLight.h"
#include "LightManager.h"

void DirectionalLight::Enable(bool enable)
{
	if(enable == mIsEnabled)
	{
		return;
	}
	else
	{
		if(enable == true)
		{
			if(LightManager::numActiveDirectionalLights < MAX_NUM_DIRECTIONAL_LIGHTS)
			{
				mIsEnabled = true;
				LightManager::numActiveDirectionalLights++;

				LightManager::isDirectionalLightsDirty = true;
			}
		}
		else
		{
			mIsEnabled = false;
			LightManager::numActiveDirectionalLights--;

			LightManager::isDirectionalLightsDirty = true;
		}
	}
}

bool DirectionalLight::IsEnabled()
{
	return mIsEnabled;
}

void DirectionalLight::SetID(DWORD id)
{
	mID = id;
}

void DirectionalLight::SetValue(D3DXCOLOR color, Vector3 dir, float intensity)
{
	mColor = color;
	mDirection = dir;
	mIntensity = intensity;

	LightManager::isDirectionalLightsDirty = true;
}

wchar_t* DirectionalLight::GetName()
{
	return mName;
}