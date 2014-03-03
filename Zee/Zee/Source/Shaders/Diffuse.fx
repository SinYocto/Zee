// Diffuse.fx
#include "CommonEnv.h"

float4x4 matWVP;
float4x4 matWorld;
float4x4 matUVTransform;

float4 mtlAmbient;
float4 mtlDiffuse;

bool useColorTex;
texture colorTex;

bool calcShadow;
texture shadowTex;

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

sampler ShadowS = sampler_state
{
	Texture = <shadowTex>;
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
	float3 posRaw : TEXCOORD3;
};

VS_OUT DiffuseVS(VS_IN vIn)
{
	VS_OUT vOut;

	vOut.pos = mul(float4(vIn.pos, 1.0f), matWVP);
	vOut.posW = (mul(float4(vIn.pos, 1.0f), matWorld)).xyz;
	vOut.tex = (mul(float4(vIn.tex, 0, 1.0f), matUVTransform)).xy;
	vOut.normal = (mul(float4(vIn.normal, 0), matWorld)).xyz;
	vOut.posRaw = vIn.pos;

	return vOut;
}

float4 DiffusePS(float2 tex : TEXCOORD0, 
				 float3 normal : TEXCOORD1, 
				 float3 posW : TEXCOORD2,
				 float3 posRaw : TEXCOORD3
				 ) : COLOR0
{
	float4 oColor = float4(0, 0, 0, 1);
	
	normal = normalize(normal); 

	float4 Ka = mtlAmbient;
	float4 Kd = mtlDiffuse;

	if(useColorTex)
	{
		float4 texColor = tex2D(ColorS, tex);

		Ka *= texColor;
		Kd *= texColor;
	}
	
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

	float shadow = 1.0f;
	if(calcShadow)
	{
		float4 posClip = mul(float4(posRaw, 1.0f), matWVP);
		posClip.xyz /= posClip.w;

		float2 posTex = float2(posClip.x * 0.5 + 0.5, -posClip.y * 0.5 + 0.5);
		shadow = tex2D(ShadowS, posTex).x;
	}

	oColor.rgb *= shadow;

	return oColor;
}

technique Diffuse
{
	pass P0
	{
		VertexShader = compile vs_3_0 DiffuseVS();
		PixelShader = compile ps_3_0 DiffusePS();		
	}
}
