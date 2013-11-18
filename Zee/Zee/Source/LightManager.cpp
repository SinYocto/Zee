#include "LightManager.h"

LightManager::LightManager()
:mAmbientLight(NULL)
,curDirLightID(0)
,curPointLightID(0)
,numActiveDirectionalLights(0)
,numActivePointLights(0)
,isDirectionalLightsDirty(true)
,isPointLightsDirty(true)
{

}

LightManager::~LightManager()
{
	Destroy();
}


void LightManager::Init()
{
	mAmbientLight = new AmbientLight();
}

void LightManager::Destroy()
{
	for(std::list<DirectionalLight*>::iterator iter = mDirlLights.begin(); iter != mDirlLights.end(); ++iter)
	{
		SAFE_DELETE(*iter);
	}

	mDirlLights.clear();

	for(std::list<DirectionalLight*>::iterator iter = mDirlLights.begin(); iter != mDirlLights.end(); ++iter)
	{
		SAFE_DELETE(*iter);
	}

	mPointLights.clear();

	SAFE_DELETE(mAmbientLight);
}

void LightManager::AddDirectionalLight(DirectionalLight* light)
{
	_Assert(NULL != light);
	light->SetID(curDirLightID++);			// QUESTION:ID一直加不会溢出吧

	mDirlLights.push_back(light);

	if(light->IsEnabled())
	{
		if(numActiveDirectionalLights < MAX_NUM_DIRECTIONAL_LIGHTS)
			numActiveDirectionalLights++;
		else
			light->Enable(false);
	}
}

void LightManager::AddPointLight(PointLight* light)
{
	_Assert(NULL != light);
	light->SetID(curPointLightID++);			// QUESTION:ID一直加不会溢出吧

	mPointLights.push_back(light);

	if(light->IsEnabled())
	{
		if(numActivePointLights < MAX_NUM_POINT_LIGHTS)
			numActivePointLights++;
		else
			light->Enable(false);
	}
}

void LightManager::FrameUpdate()
{
	if(isDirectionalLightsDirty)
	{
		for(int i = 0; i < MAX_NUM_DIRECTIONAL_LIGHTS; ++i)
		{
			directionalLightsData[i] = DirectionalLightData();		// TODO:改成clear函数
		}

		int ix = 0;
		for(std::list<DirectionalLight*>::iterator iter = mDirlLights.begin(); iter != mDirlLights.end(); ++iter)
		{
			if((*iter)->IsEnabled())
			{
				directionalLightsData[ix] = (*iter)->GetData();
				ix++;
			}
		}

		isDirectionalLightsDirty = false;
	}

	if(isPointLightsDirty){

		for(int i = 0; i < MAX_NUM_POINT_LIGHTS; ++i)
		{
			pointLightsData[i] = PointLightData();
		}

		int ix = 0;
		for(std::list<PointLight*>::iterator iter = mPointLights.begin(); iter != mPointLights.end(); ++iter)
		{
			if((*iter)->IsEnabled())
			{
				pointLightsData[ix] = (*iter)->GetData();
				ix++;
			}
		}

		isPointLightsDirty = false;
	}
}

void LightManager::SetAmbientLight(D3DXCOLOR _color, float _intensity)
{
	mAmbientLight->SetValue(_color, _intensity);
}

D3DXCOLOR LightManager::GetFinalAmbientColor()
{
	return mAmbientLight->FinalColor();
}

void LightManager::GetDirLight(const wchar_t* name, DirectionalLight** dirLight)
{
	_Assert(NULL != dirLight);

	*dirLight = NULL;
	for(std::list<DirectionalLight*>::iterator iter = mDirlLights.begin(); iter != mDirlLights.end(); ++iter)
	{
		DirectionalLight* curDirLight = *iter;
		if(YString::Compare(curDirLight->GetName(), name) == 0)
		{
			*dirLight = curDirLight;
			break;
		}
	}
}

void LightManager::GetPointLight(const wchar_t* name, PointLight** pointLight)
{
	_Assert(NULL != pointLight);

	*pointLight = NULL;
	for(std::list<PointLight*>::iterator iter = mPointLights.begin(); iter != mPointLights.end(); ++iter)
	{
		PointLight* curPointLight = *iter;
		if(YString::Compare(curPointLight->GetName(), name) == 0)
		{
			*pointLight = curPointLight;
			break;
		}
	}
}

void LightManager::EnableOneDirectionalLight(bool enable)
{
	if(enable)
	{
		numActiveDirectionalLights++;
		SetDirectionalLightDirtyFlag(true);
	}
	else
	{
		numActiveDirectionalLights--;
		SetDirectionalLightDirtyFlag(true);
	}
}

void LightManager::EnableOnePointLight( bool enable )
{
	if(enable)
	{
		numActivePointLights++;
		SetPointLightDirtyFlag(true);
	}
	else
	{
		numActivePointLights--;
		SetPointLightDirtyFlag(true);
	}
}


void LightManager::SetDirectionalLightDirtyFlag(bool isDirty)
{
	isDirectionalLightsDirty = isDirty;
}

void LightManager::SetPointLightDirtyFlag(bool isDirty)
{
	isPointLightsDirty = isDirty;
}

DirectionalLightData* LightManager::GetDirectionalLightsData()
{
	return directionalLightsData;
}

PointLightData* LightManager::GetPointLightsData()
{
	return pointLightsData;
}

int LightManager::GetActiveDirectionalLightCounts()
{
	return numActiveDirectionalLights;
}

int LightManager::GetActivePointLightCounts()
{
	return numActivePointLights;
}
