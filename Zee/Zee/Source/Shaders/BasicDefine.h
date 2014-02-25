#ifndef BASIC_DEFINE_H
#define BASIC_DEFINE_H

#define MAX_NUM_DIRECTIONAL_LIGHTS 1
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

float GetAlphaFromColor(float3 color)
{
	float alpha = 0;

	if(color.r > alpha)
		alpha = color.r;

	if(color.g > alpha)
		alpha = color.g;

	if(color.b > alpha)
		alpha = color.b;

	return alpha;
}

#endif