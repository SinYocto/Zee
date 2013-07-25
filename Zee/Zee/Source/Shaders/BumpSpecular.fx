// BumpSpecular.fx
// using NormalMap

#include"LightsDefine.fx"

float4x4 matWVP;
float4x4 matWorld;
float4x4 matUVTransform;

texture colorTex;
texture normalTex;
float4 mtlDiffuse;
float4 mtlSpec;
float gloss;

float3 eyePos;

AmbientLight ambientLight;
DirectionalLight directionalLights[MAX_NUM_DIRECTIONAL_LIGHTS];
PointLight pointLights[MAX_NUM_POINT_LIGHTS];

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

sampler NormalS = sampler_state
{
	Texture = <normalTex>;
	MinFilter = linear;
	MaxAnisotropy = 4;
	MagFilter = linear;
	MipFilter = linear;
	AddressU  = WRAP;
    AddressV  = WRAP;
};

void BumpSpecularVS(float3 Pos : POSITION0, 
			float2 Tex : TEXCOORD0,
			float3 Tangent : TANGENT0,
			float3 Bitangent : BINORMAL0,
			float3 Normal : NORMAL0,
			out float4 oPos  : POSITION0, 
			out float2 oTex : TEXCOORD0,
			out float3 oTangent : TEXCOORD1,
			out float3 oBitangent : TEXCOORD2,
			out float3 oNormal : TEXCOORD3,
			out float3 oPosW : TEXCOORD4)
{
	oPos = mul(float4(Pos, 1.0f), matWVP);
	oPosW = (mul(float4(Pos, 1.0f), matWorld)).xyz;
	oTex = (mul(float4(Tex, 0, 1.0f), matUVTransform)).xy;
	oTangent = (mul(float4(Tangent, 0), matWorld)).xyz;
	oBitangent = (mul(float4(Bitangent, 0), matWorld)).xyz;
	oNormal = (mul(float4(Normal, 0), matWorld)).xyz;
}

float4 BumpSpecularPS(float2 Tex : TEXCOORD0, 
				float3 Tangent : TEXCOORD1, 
				float3 Bitangent : TEXCOORD2, 
				float3 Normal : TEXCOORD3, 
				float3 posW : TEXCOORD4) : COLOR0
{
	float4 totalDiffuse = float4(0, 0, 0, 1);
	float4 totalSpec = float4(0, 0, 0, 1);
	
	float3 eyeDir = normalize(eyePos - posW);
	
	float3x3 TBN;
	TBN[0] = Tangent;
	TBN[1] = Bitangent;
	TBN[2] = Normal;
	
	Normal = 2*tex2D(NormalS, Tex) - 1.0f;
	Normal = mul(Normal, TBN);
	Normal = normalize(Normal);

	for(int i = 0; i < MAX_NUM_DIRECTIONAL_LIGHTS; ++i)
	{
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
	
	for(int i = 0; i < MAX_NUM_POINT_LIGHTS; ++i)
	{
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
	
	float4 color = (ambientLight.color + mtlDiffuse * totalDiffuse) * tex2D(ColorS, Tex) + mtlSpec * totalSpec;
	return color;
}

technique BumpSpecular
{
	pass P0
	{
		VertexShader = compile vs_3_0 BumpSpecularVS();
		PixelShader = compile ps_3_0 BumpSpecularPS();		
	}
}
