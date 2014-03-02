// ShadowTex.fx

float4x4 matWVP;
float4x4 matWorld;
float4x4 matLightVP;

float3 lightPos;

texture	shadowMapTex;

sampler ShadowMapS = sampler_state
{
	Texture = <shadowMapTex>;
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
};

struct VS_OUT
{
	float4 pos : POSITION0;
	float3 posW : TEXCOORD0;
};

VS_OUT ShadowVS(VS_IN vIn)
{
	VS_OUT vOut;

	vOut.pos = mul(float4(vIn.pos, 1.0f), matWVP);
	vOut.posW = (mul(float4(vIn.pos, 1.0f), matWorld)).xyz;

	return vOut;
}

float4 ShadowPS(float3 posW : TEXCOORD0) : COLOR0		
{
	float shadow = 0;

	float2 shadowMapValues;
	{
		float4 posLightClip  = mul(float4(posW, 1.0f), matLightVP);	
		posLightClip.xyz /= posLightClip.w;

		float2 shadowMapTexcoord = float2(posLightClip.x * 0.5 + 0.5, -posLightClip.y * 0.5 + 0.5);

		shadowMapValues = tex2D(ShadowMapS, shadowMapTexcoord).xy;
	}
	
	float pixelDepth = length(lightPos - posW.xyz);

	if(pixelDepth < shadowMapValues.x + 0.5 f)
	{
		shadow = 1;
	}
	else
	{
		shadow = 0;
	}

	return float4(shadow, shadow, shadow, 1);
}

technique Shadow
{
	pass P0
	{
		VertexShader = compile vs_3_0 ShadowVS();
		PixelShader = compile ps_3_0 ShadowPS();		
	}
}

