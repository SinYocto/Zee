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

void SpecularVS(float3 Pos : POSITION0, 
			float2 Tex : TEXCOORD0,
			float3 Normal : NORMAL0,
			out float4 oPos  : POSITION0, 
			out float2 oTex : TEXCOORD0,
			out float3 oNormal : TEXCOORD1,
			out float3 oPosW : TEXCOORD2)
{
	oPos = mul(float4(Pos, 1.0f), matWVP);
	oPosW = (mul(float4(Pos, 1.0f), matWorld)).xyz;
	oTex = (mul(float4(Tex, 0, 1.0f), matUVTransform)).xy;
	oNormal = (mul(float4(Normal, 0), matWorld)).xyz;
}

float4 SpecularPS(float2 Tex : TEXCOORD0, 
				  float3 Normal : TEXCOORD1, 
				  float3 posW : TEXCOORD2) : COLOR0
{
	float4 totalDiffuse = float4(0, 0, 0, 1);
	float4 totalSpec = float4(0, 0, 0, 1);
	
	float3 eyeDir = normalize(eyePos - posW);
	Normal = normalize(Normal);
	
	for(int i = 0; i < MAX_NUM_DIRECTIONAL_LIGHTS; ++i){
		float3 lightDir = normalize(-directionalLights[i].dir);
		float3 halfVec = normalize(lightDir + eyeDir);
		
		float diffuse = saturate(dot(lightDir, Normal)); 
		float spec;
		if(dot(lightDir, Normal) > 0)
			spec = pow(saturate(dot(halfVec, Normal)), gloss);
		else
			spec = 0;
		
		totalDiffuse += diffuse * directionalLights[i].color;
		totalSpec += spec * directionalLights[i].color;
	}
	
	for(int i = 0; i < MAX_NUM_POINT_LIGHTS; ++i){
		float3 lightDir = normalize(pointLights[i].position - posW);
		float3 halfVec = normalize(lightDir + eyeDir);
		
		float diffuse = saturate(dot(lightDir, Normal));
		float spec;
		if(dot(lightDir, Normal) > 0)
			spec = pow(saturate(dot(halfVec, Normal)), gloss);
		else
			spec = 0;
			
		float distance = length(posW - pointLights[i].position);
		float attenuation = 1 / (pointLights[i].atten.x + pointLights[i].atten.y * distance + pointLights[i].atten.z * distance * distance);
		
		totalDiffuse += attenuation * diffuse * pointLights[i].color;
		totalSpec += attenuation * spec * pointLights[i].color;
	}

	float4 texColor = float4(1, 1, 1, 1);
	if(useColorTex)
		texColor = tex2D(ColorS, Tex);
	
	float4 color = (mtlAmbient * ambientLight.color + mtlDiffuse * totalDiffuse) * texColor + mtlSpec * totalSpec;
	return color;
}

technique Specular
{
	pass P0
	{
		VertexShader = compile vs_3_0 SpecularVS();
		PixelShader = compile ps_3_0 SpecularPS();		
	}
}
