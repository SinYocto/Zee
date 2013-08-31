// Billboard.fx
#include "BasicDefine.h"

float4x4 matWVP;
float4 mtlColor;

texture colorTex;
bool isAlphaFromColor;

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
};

struct VS_OUT
{
	float4 pos : POSITION0;
	float2 tex : TEXCOORD0;
};

VS_OUT BillboardVS(VS_IN vIn)
{
	VS_OUT vOut;

	vOut.pos = mul(float4(vIn.pos, 1.0f), matWVP);
	vOut.tex = vIn.tex;

	return vOut;
}

float4 BillboardPS(float2 tex : TEXCOORD0) : COLOR0
{
	float4 texColor = tex2D(ColorS, tex);

	if(isAlphaFromColor)
		texColor.a = GetAlphaFromColor(texColor.rgb);

	float4 oColor = mtlColor * texColor;
	return oColor;
}

technique Billboard
{
	pass P0
	{
		VertexShader = compile vs_3_0 BillboardVS();
		PixelShader = compile ps_3_0 BillboardPS();		
	}
}
