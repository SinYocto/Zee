// Terrain.fx
#include "CommonEnv.h"

float4x4 matWVP;
float4x4 matWorld;
float4x4 matUVTransform;

float4 mtlAmbient;
float4 mtlDiffuse;

bool drawBrushTex;
float4 brushRect;

texture colorTex0;
texture colorTex1;
texture colorTex2;
texture colorTex3;
texture splatMapTex;
texture brushTex;

bool calcShadow;
texture shadowTex;

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

sampler BrushS = sampler_state
{
	Texture = <brushTex>;
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
	float2 texSplat : TEXCOORD1;
	float3 normal : TEXCOORD2;
	float3 posW : TEXCOORD3;
	float3 posRaw : TEXCOORD4;
};


VS_OUT TerrainVS(VS_IN vIn)
{
	VS_OUT vOut;

	vOut.pos = mul(float4(vIn.pos, 1.0f), matWVP);
	vOut.posW = (mul(float4(vIn.pos, 1.0f), matWorld)).xyz;
	vOut.tex = (mul(float4(vIn.tex, 0, 1.0f), matUVTransform)).xy; 
	vOut.normal = (mul(float4(vIn.normal, 0), matWorld)).xyz;
	vOut.texSplat = vIn.tex;
	vOut.posRaw = vIn.pos;

	return vOut;
}

float4 TerrainPS(float2 tex : TEXCOORD0, 
				 float2 texSplat : TEXCOORD1, 
				 float3 normal : TEXCOORD2, 
				 float3 posW : TEXCOORD3,
				 float3 posRaw : TEXCOORD4) : COLOR0
{
	float4 oColorAmbient = float4(0, 0, 0, 1);
	float4 oColorPointLights = float4(0, 0, 0, 1);
	float4 oColor = float4(0, 0, 0, 1);
	
	normal = normalize(normal);
	
	float4 splatColor = tex2D(SplatS, texSplat);
	float4 texColor = splatColor.r * tex2D(ColorS0, tex) + splatColor.g * tex2D(ColorS1, tex) + 
		splatColor.b * tex2D(ColorS2, tex) + splatColor.a * tex2D(ColorS3, tex);
	
	float4 Ka = mtlAmbient * texColor;
	float4 Kd = mtlDiffuse * texColor;
	
	CalcORadianceAmbient(oColorAmbient, ambientLight.color, Ka);	

	for(int i = 0; i < MAX_NUM_DIRECTIONAL_LIGHTS; ++i)
	{
		CalcORadianceLambert(oColor, directionalLights[i].color, -directionalLights[i].dir, normal, Kd);
	}
	
	for(int i = 0; i < MAX_NUM_POINT_LIGHTS; ++i)
	{	
		float3 dirL = normalize(pointLights[i].position - posW);
		float atten = CalcAttenuation(posW, pointLights[i].position, pointLights[i].atten);

		CalcORadianceLambert(oColorPointLights, atten * pointLights[i].color, dirL, normal, Kd);
	}

	float shadow = 1.0f;
	if(calcShadow)
	{
		float4 posClip = mul(float4(posRaw, 1.0f), matWVP);
		posClip.xyz /= posClip.w;

		float2 posTex = float2(posClip.x * 0.5 + 0.5, -posClip.y * 0.5 + 0.5);
		shadow = tex2D(ShadowS, posTex).x;
	}

	shadow = clamp(shadow, 0, 1.0f);

	oColor.rgb = oColorAmbient.rgb + oColorPointLights.rgb + shadow * oColor.rgb;

	// ¼ÓÉÏbrushÌùÍ¼
	if(drawBrushTex)
	{
		float2 brushTexUV;
		brushTexUV.x = (texSplat.x - brushRect.x) / (brushRect.z - brushRect.x);
		brushTexUV.y = (texSplat.y - brushRect.y) / (brushRect.w - brushRect.y);

		if(brushTexUV.x > 0 && brushTexUV.x < 1 && brushTexUV.y > 0 && brushTexUV.y < 1)
		{
			float3 brushColor = tex2D(BrushS, brushTexUV).rgb;
			float alpha = GetAlphaFromColor(brushColor);
			oColor.rgb = alpha * float3(0.36f, 0.655f, 0.729f) * brushColor + (1.0f - alpha) * oColor.rgb;
		}
	}

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
