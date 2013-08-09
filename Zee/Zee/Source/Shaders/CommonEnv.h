#ifndef COMMONENV_H
#define COMMONENV_H

#include"LightsDefine.h"

shared AmbientLight ambientLight;
shared DirectionalLight directionalLights[MAX_NUM_DIRECTIONAL_LIGHTS];
shared PointLight pointLights[MAX_NUM_POINT_LIGHTS];

void CalORadianceAmbient(inout float4 oRadiance, AmbientLight ambientLight, float4 Ka)
{
	oRadiance += Ka * ambientLight.color;
}

void CalDirLightORadianceDiff(INOUT float4 oRadiance, DirectionalLight dirLight, float3 normal, float4 Kd)
{
	float3 lightDir = normalize(-dirLight.dir);
	float cosTheta = saturate(dot(lightDir, normal)); 

	oRadiance += Kd * dirLight.color * cosTheta;
}

#endif