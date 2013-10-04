// Terrain.fx
#include "CommonEnv.h"

float4x4 matWVP;
float4x4 matWorld;
float4x4 matUVTransform;

float4 mtlAmbient;
float4 mtlDiffuse;

texture colorTex0;
texture colorTex1;
texture colorTex2;
texture colorTex3;
texture splatMapTex;

sampler ColorS0 = sampler_state
{
	Texture = <colorTex0>;
	MinFilter = linear;
	MaxAnisotropy = 4;
	MagFilter = linear;
	MipFilter = linear;
	AddressU  = WRAP;
    AddressV  = WRAP;
};

sampler ColorS1 = sampler_state
{
	Texture = <colorTex1>;
	MinFilter = linear;
	MaxAnisotropy = 4;
	MagFilter = linear;
	MipFilter = linear;
	AddressU  = WRAP;
    AddressV  = WRAP;
};

sampler ColorS2 = sampler_state
{
	Texture = <colorTex2>;
	MinFilter = linear;
	MaxAnisotropy = 4;
	MagFilter = linear;
	MipFilter = linear;
	AddressU  = WRAP;
    AddressV  = WRAP;
};

sampler ColorS3 = sampler_state
{
	Texture = <colorTex3>;
	MinFilter = linear;
	MaxAnisotropy = 4;
	MagFilter = linear;
	MipFilter = linear;
	AddressU  = WRAP;
    AddressV  = WRAP;
};

sampler SplatS = sampler_state
{
	Texture = <splatMapTex>;
	MinFilter = linear;
	MaxAnisotropy = 4;
	MagFilter = linear;
	MipFilter = linear;
	AddressU  = WRAP;
    AddressV  = WRAP;
};

struct VS_IN
{
	float3 pos : POSITION0;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL0;
};

struct VS_OUT
{
	float4 pos : POSITION0;
	float2 tex : TEXCOORD0;
	float2 texSplat : TEXCOORD1;
	float3 normal : TEXCOORD2;
	float3 posW : TEXCOORD3;
};


VS_OUT TerrainVS(VS_IN vIn)
{
	VS_OUT vOut;

	vOut.pos = mul(float4(vIn.pos, 1.0f), matWVP);
	vOut.posW = (mul(float4(vIn.pos, 1.0f), matWorld)).xyz;
	vOut.tex = (mul(float4(vIn.tex, 0, 1.0f), matUVTransform)).xy; 
	vOut.normal = (mul(float4(vIn.normal, 0), matWorld)).xyz;
	vOut.texSplat = vIn.tex;

	return vOut;
}

float4 TerrainPS(float2 tex : TEXCOORD0, 
				 float2 texSplat : TEXCOORD1, 
				 float3 normal : TEXCOORD2, 
				 float3 posW : TEXCOORD3) : COLOR0
{
	float4 oColor =  float4(0, 0, 0, 1);
	
	normal = normalize(normal);
	
	float4 splatColor = tex2D(SplatS, texSplat);
	float4 texColor = splatColor.r * tex2D(ColorS0, tex) + splatColor.g * tex2D(ColorS1, tex) + 
		splatColor.b * tex2D(ColorS2, tex) + splatColor.a * tex2D(ColorS3, tex);
	
	float4 Ka = mtlAmbient * texColor;
	float4 Kd = mtlDiffuse * texColor;
	
	CalcORadianceAmbient(oColor, ambientLight.color, Ka);	

	for(int i = 0; i < MAX_NUM_DIRECTIONAL_LIGHTS; ++i)
	{
		CalcORadianceLambert(oColor, directionalLights[i].color, -directionalLights[i].dir, normal, Kd);
	}
	
	for(int i = 0; i < MAX_NUM_POINT_LIGHTS; ++i)
	{	
		float3 dirL = normalize(pointLights[i].position - posW);
		float atten = CalcAttenuation(posW, pointLights[i].position, pointLights[i].atten);

		CalcORadianceLambert(oColor, atten * pointLights[i].color, dirL, normal, Kd);
	}

	//oColor = ambientLight.color * Ka;

	return oColor;
}

technique Terrain
{
	pass P0
	{
		VertexShader = compile vs_3_0 TerrainVS();
		PixelShader = compile ps_3_0 TerrainPS();		
	}
}
