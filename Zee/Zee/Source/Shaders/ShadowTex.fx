// ShadowTex.fx

float4x4 matWVP;
float4x4 matWorld;
float4x4 matLightVP;

float3 lightPos;

texture	shadowMapTex;

const float MIN_VARIANCE = 0.4f;
const float THRESHOLD = 0.2f;

sampler ShadowMapS = sampler_state
{
	Texture = <shadowMapTex>;
	MinFilter = anisotropic;
	MaxAnisotropy = 8;
	MagFilter = linear;
	MipFilter = linear;
	AddressU  = Clamp;
    AddressV  = Clamp;
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

	if(pixelDepth < shadowMapValues.x)
	{
		shadow = 1;
	}
	else
	{
		float variance = shadowMapValues.y - shadowMapValues.x * shadowMapValues.x;
		variance = clamp(variance, MIN_VARIANCE, 10000.0f);
		shadow = variance / (variance + (pixelDepth - shadowMapValues.x)*(pixelDepth - shadowMapValues.x));
		shadow = clamp((shadow - THRESHOLD)/(1 - THRESHOLD), 0, 1.0f);
		//shadow = 0;
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

