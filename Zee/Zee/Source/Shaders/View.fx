// View.fx
#include "CommonEnv.h"

float4x4 matWVP;
float4x4 matWorld;
float4x4 matUVTransform;

float4 mtlColor;

bool useColorTex;
texture colorTex;

float3 eyePos;

sampler ColorS = sampler_state
{
	Texture = <colorTex>;
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
	float3 normal : TEXCOORD1;
	float3 posW : TEXCOORD2;
};

VS_OUT ViewVS(VS_IN vIn)
{
	VS_OUT vOut;

	vOut.pos = mul(float4(vIn.pos, 1.0f), matWVP);
	vOut.posW = (mul(float4(vIn.pos, 1.0f), matWorld)).xyz;
	vOut.tex = (mul(float4(vIn.tex, 0, 1.0f), matUVTransform)).xy;
	vOut.normal = (mul(float4(vIn.normal, 0), matWorld)).xyz;

	return vOut;
}

float4 ViewPS(float2 tex : TEXCOORD0, 
			  float3 normal : TEXCOORD1, 
			  float3 posW : TEXCOORD2) : COLOR0
{
	float4 oColor = float4(0, 0, 0, 1);
	
	float3 dirV = normalize(eyePos - posW);
	normal = normalize(normal);

	float4 Kd = mtlColor;

	if(useColorTex)
	{
		Kd *= tex2D(ColorS, tex);
	}

	CalcORadianceLambert(oColor, float4(1, 1, 1, 1), dirV, normal, Kd);

	return oColor;
}

technique View
{
	pass P0
	{
		VertexShader = compile vs_3_0 ViewVS();
		PixelShader = compile ps_3_0 ViewPS();		
	}
}
