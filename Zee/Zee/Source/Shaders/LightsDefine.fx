// LightsDefine.fx
#define MAX_NUM_DIRECTIONAL_LIGHTS 2
#define MAX_NUM_POINT_LIGHTS 8

struct DirectionalLight
{
	float4 color;
	float3 dir;
//	float pad;
};

struct PointLight
{
	float4 color;
	float3 position;
//	float pad1;
	float3 atten;
//	float pad2;
};

struct AmbientLight
{
	float4 color;
};