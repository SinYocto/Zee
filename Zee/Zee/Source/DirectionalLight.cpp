#include"DirectionalLight.h"
#include "Engine.h"

void DirectionalLight::Enable(bool enable)
{
	if(enable == mIsEnabled)
	{
		return;
	}
	else
	{
		LightManager* lightMgr = gEngine->GetLightManager();
		if(enable == true)
		{
			if(lightMgr->GetActiveDirectionalLightCounts() < MAX_NUM_DIRECTIONAL_LIGHTS)
			{
				mIsEnabled = true;
				lightMgr->EnableOneDirectionalLight(true);
			}
		}
		else
		{
			mIsEnabled = false;
			lightMgr->EnableOneDirectionalLight(false);
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

	gEngine->GetLightManager()->SetDirectionalLightDirtyFlag(true);
}

wchar_t* DirectionalLight::GetName()
{
	return mName;
}

Vector3 DirectionalLight::GetDirection()
{
	return mDirection;
}

void DirectionalLight::SetDirection(const Vector3& dir)
{
	mDirection = dir;

	gEngine->GetLightManager()->SetDirectionalLightDirtyFlag(true);
}

D3DXCOLOR DirectionalLight::GetFinalColor()
{
	return D3DXCOLOR(mIntensity * mColor.r, mIntensity * mColor.g, mIntensity * mColor.b, mColor.a);
}

float DirectionalLight::GetIntensity()
{
	return mIntensity;
}

D3DXCOLOR DirectionalLight::GetColor()
{
	return mColor;
}

void DirectionalLight::SetIntensity(float intensity)
{
	mIntensity = intensity;
	gEngine->GetLightManager()->SetDirectionalLightDirtyFlag(true);
}

void DirectionalLight::SetColor(D3DXCOLOR color)
{
	mColor = color;
	gEngine->GetLightManager()->SetDirectionalLightDirtyFlag(true);
}
