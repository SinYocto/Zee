#ifndef COMMONENV_H
#define COMMONENV_H

#include"BasicDefine.h"

shared AmbientLight ambientLight;
shared DirectionalLight directionalLights[MAX_NUM_DIRECTIONAL_LIGHTS];
shared PointLight pointLights[MAX_NUM_POINT_LIGHTS];

void CalcORadianceAmbient(inout float4 oRadiance, float4 ambientRadiance, float4 Ka)
{
	oRadiance += Ka * ambientRadiance;
}

void CalcORadianceLambert(inout float4 oRadiance, float4 lRadiance, float3 dirL, float3 normal, float4 Kd)
{
	float cosTi = saturate(dot(dirL, normal)); 

	oRadiance += Kd * lRadiance * cosTi;
}

void CalcORadianceBlinnPhong(inout float4 oRadiance, float4 lRadiance, float3 dirL, float3 normal, 
								  float3 dirV, float4 Kd, float4 Ks, float Ns)
{
	float3 halfVec = normalize(dirL + dirV);

	float cosTi = saturate(dot(dirL, normal)); 
	float cosTh = saturate(dot(halfVec, normal));

	oRadiance += (Kd + Ks * pow(cosTh, Ns)) * lRadiance * cosTi;
}

float CalcAttenuation(float3 posW, float3 posL, float3 attenFactor)
{
	float dist = length(posW - posL);
	return 1 / (attenFactor.x + attenFactor.y * dist + attenFactor.z * dist * dist);
}

#endif