#include"PointLight.h"
#include "Engine.h"

void PointLight::Enable(bool enable)
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
			if(lightMgr->GetActivePointLightCounts() < MAX_NUM_POINT_LIGHTS)
			{
				mIsEnabled = true;
				lightMgr->EnableOnePointLight(true);
			}
		}
		else
		{
			mIsEnabled = false;
			lightMgr->EnableOnePointLight(false);
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

	gEngine->GetLightManager()->SetPointLightDirtyFlag(true);
}

wchar_t* PointLight::GetName()
{
	return mName;
}

void PointLight::SetPosition(const Vector3& pos)
{
	mPosition = pos;
	gEngine->GetLightManager()->SetPointLightDirtyFlag(true);
}

Vector3 PointLight::GetPosition()
{
	return mPosition;
}

D3DXCOLOR PointLight::GetFinalColor()
{
	return D3DXCOLOR(mIntensity * mColor.r, mIntensity * mColor.g, mIntensity * mColor.b, mColor.a); 
}

D3DXCOLOR PointLight::GetColor()
{
	return mColor;
}

float PointLight::GetIntensity()
{
	return mIntensity;
}

void PointLight::SetIntensity(float intensity)
{
	mIntensity = intensity;
	gEngine->GetLightManager()->SetPointLightDirtyFlag(true);
}	

void PointLight::SetColor(D3DXCOLOR color)
{
	mColor = color;
	gEngine->GetLightManager()->SetPointLightDirtyFlag(true);
}
