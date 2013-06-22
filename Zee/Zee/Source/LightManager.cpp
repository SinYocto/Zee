#include "LightManager.h"

int LightManager::numActiveDirectionalLights = 0;
int LightManager::numActivePointLights = 0;

AmbientLight LightManager::ambientLight;
std::list<DirectionalLight*> LightManager::directionalLights;
std::list<PointLight*> LightManager::pointLights;

DirectionalLightData LightManager::directionalLightsData[MAX_NUM_DIRECTIONAL_LIGHTS];
PointLightData LightManager::pointLightsData[MAX_NUM_POINT_LIGHTS];

bool LightManager::isDirectionalLightsDirty = true;
bool LightManager::isPointLightsDirty = true;


void LightManager::AddDirectionalLight(DirectionalLight* light)
{
	directionalLights.push_back(light);

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
	pointLights.push_back(light);

	if(light->IsEnabled())
	{
		if(numActivePointLights < MAX_NUM_POINT_LIGHTS)
			numActivePointLights++;
		else
			light->Enable(false);
	}
}

void LightManager::Update()
{
	if(isDirectionalLightsDirty)
	{
		for(int i = 0; i < MAX_NUM_DIRECTIONAL_LIGHTS; ++i)
		{
			directionalLightsData[i] = DirectionalLightData();		// TODO:¸Ä³Éclearº¯Êý
		}

		int ix = 0;
		for(std::list<DirectionalLight*>::iterator iter = directionalLights.begin(); iter != directionalLights.end(); ++iter)
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
		for(std::list<PointLight*>::iterator iter = pointLights.begin(); iter != pointLights.end(); ++iter)
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
	ambientLight.SetValue(_color, _intensity);
}

D3DXCOLOR LightManager::GetFinalAmbientColor()
{
	return ambientLight.FinalColor();
}
