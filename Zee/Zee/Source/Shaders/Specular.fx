// Specular.fx
// blinn-phong

#include "CommonEnv.h"

float4x4 matWVP;
float4x4 matWorld;
float4x4 matUVTransform;

bool useColorTex;
texture colorTex;

float4 mtlAmbient;
float4 mtlDiffuse;
float4 mtlSpec;
float gloss;

float3 eyePos;

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

VS_OUT SpecularVS(VS_IN vIn)
{
	VS_OUT vOut;

	vOut.pos = mul(float4(vIn.pos, 1.0f), matWVP);
	vOut.posW = (mul(float4(vIn.pos, 1.0f), matWorld)).xyz;
	vOut.tex = (mul(float4(vIn.tex, 0, 1.0f), matUVTransform)).xy;
	vOut.normal = (mul(float4(vIn.normal, 0), matWorld)).xyz;
	vOut.posRaw = vIn.pos;

	return vOut;
}

float4 SpecularPS(float2 tex : TEXCOORD0, 
				  float3 normal : TEXCOORD1, 
				  float3 posW : TEXCOORD2,
				  float3 posRaw : TEXCOORD3) : COLOR0
{
	float4 oColorAmbient = float4(0, 0, 0, 1);
	float4 oColor = float4(0, 0, 0, 1);

	float3 dirV = normalize(eyePos - posW);
	normal = normalize(normal);
	
	float4 Ka = mtlAmbient;
	float4 Kd = mtlDiffuse;
	float4 Ks = mtlSpec;
	float Ns = gloss;

	if(useColorTex)
	{
		float4 texColor = tex2D(ColorS, tex);

		Ka *= texColor;
		Kd *= texColor;
	}
	
	CalcORadianceAmbient(oColorAmbient, ambientLight.color, Ka);

	for(int i = 0; i < MAX_NUM_DIRECTIONAL_LIGHTS; ++i)
	{
		CalcORadianceBlinnPhong(oColor, directionalLights[i].color, -directionalLights[i].dir, normal, dirV, Kd, Ks, Ns);
	}
	
	for(int i = 0; i < MAX_NUM_POINT_LIGHTS; ++i)
	{	
		float3 dirL = normalize(pointLights[i].position - posW);
		float atten = CalcAttenuation(posW, pointLights[i].position, pointLights[i].atten);
		
		CalcORadianceBlinnPhong(oColor, atten * pointLights[i].color, dirL, normal, dirV, Kd, Ks, Ns);
	}

	float shadow = 1.0f;
	if(calcShadow)
	{
		float4 posClip = mul(float4(posRaw, 1.0f), matWVP);
		posClip.xyz /= posClip.w;

		float2 posTex = float2(posClip.x * 0.5 + 0.5, -posClip.y * 0.5 + 0.5);
		shadow = tex2D(ShadowS, posTex).x;
	}

	oColor.rgb = oColorAmbient.rgb + shadow * oColor.rgb;
	return oColor;
}

technique Specular
{
	pass P0
	{
		VertexShader = compile vs_3_0 SpecularVS();
		PixelShader = compile ps_3_0 SpecularPS();		
	}
}
