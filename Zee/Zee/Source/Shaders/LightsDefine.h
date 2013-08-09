#ifndef LIGHTS_DEFINE_H
#define LIGHTS_DEFINE_H

#define MAX_NUM_DIRECTIONAL_LIGHTS 2
#define MAX_NUM_POINT_LIGHTS 8

struct DirectionalLight
{
	float4 color;
	float3 dir;
};

struct PointLight
{
	float4 color;
	float3 position;
	float3 atten;
};

struct AmbientLight
{
	float4 color;
};

#endif