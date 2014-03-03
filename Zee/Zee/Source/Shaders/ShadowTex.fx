// ShadowTex.fx

float4x4 matWVP;
float4x4 matWorld;

float4x4 matLightVP0;
float4x4 matLightVP1;
float4x4 matLightVP2;

float farZ;

float3 lightPos0;
float3 lightPos1;
float3 lightPos2;

texture	shadowMapTex0;
texture	shadowMapTex1;
texture	shadowMapTex2;

const int CASCADE_COUNTS = 3;

const float MIN_VARIANCE = 0.2f;
const float THRESHOLD = 0.2f;

sampler ShadowMapS0 = sampler_state
{
	Texture = <shadowMapTex0>;
	MinFilter = anisotropic;
	MaxAnisotropy = 8;
	MagFilter = linear;
	MipFilter = linear;
	AddressU  = Clamp;
    AddressV  = Clamp;
};

sampler ShadowMapS1 = sampler_state
{
	Texture = <shadowMapTex1>;
	MinFilter = anisotropic;
	MaxAnisotropy = 8;
	MagFilter = linear;
	MipFilter = linear;
	AddressU  = Clamp;
    AddressV  = Clamp;
};

sampler ShadowMapS2 = sampler_state
{
	Texture = <shadowMapTex2>;
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
	float3 posRaw : TEXCOORD1;
};

VS_OUT ShadowVS(VS_IN vIn)
{
	VS_OUT vOut;

	vOut.pos = mul(float4(vIn.pos, 1.0f), matWVP);
	vOut.posW = (mul(float4(vIn.pos, 1.0f), matWorld)).xyz;
	vOut.posRaw = vIn.pos;

	return vOut;
}

float4 ShadowPS(float3 posW : TEXCOORD0,
				float3 posRaw : TEXCOORD1) : COLOR0		
{
	float shadow = 0;

	int cascadeIndex = 0;
	{
		float pixelZ = mul(float4(posRaw, 1.0f), matWVP).w;

		if(pixelZ > farZ / CASCADE_COUNTS)
			cascadeIndex += 1;

		if(pixelZ > 2.0f * farZ / CASCADE_COUNTS)
			cascadeIndex += 1;
	}

	float2 shadowMapValues;
	float pixelDepth = 0;
	{
		if(cascadeIndex == 0)
		{
			float4 posLightClip  = mul(float4(posW, 1.0f), matLightVP0);	
			posLightClip.xyz /= posLightClip.w;

			float2 shadowMapTexcoord = float2(posLightClip.x * 0.5 + 0.5, -posLightClip.y * 0.5 + 0.5);

			shadowMapValues = tex2D(ShadowMapS0, shadowMapTexcoord).xy;
			pixelDepth = length(lightPos0 - posW.xyz);
		}
		
		if(cascadeIndex == 1)
		{
			float4 posLightClip  = mul(float4(posW, 1.0f), matLightVP1);	
			posLightClip.xyz /= posLightClip.w;

			float2 shadowMapTexcoord = float2(posLightClip.x * 0.5 + 0.5, -posLightClip.y * 0.5 + 0.5);

			shadowMapValues = tex2D(ShadowMapS1, shadowMapTexcoord).xy;
			pixelDepth = length(lightPos1 - posW.xyz);
		}
		
		if(cascadeIndex == 2)
		{
			float4 posLightClip  = mul(float4(posW, 1.0f), matLightVP2);	
			posLightClip.xyz /= posLightClip.w;

			float2 shadowMapTexcoord = float2(posLightClip.x * 0.5 + 0.5, -posLightClip.y * 0.5 + 0.5);

			shadowMapValues = tex2D(ShadowMapS2, shadowMapTexcoord).xy;
			pixelDepth = length(lightPos2 - posW.xyz);
		}

	}

	if(pixelDepth < shadowMapValues.x + 0.1f)
	{
		shadow = 1;
	}
	else
	{
		float variance = shadowMapValues.y - shadowMapValues.x * shadowMapValues.x;
		variance = clamp(variance, MIN_VARIANCE, 10000.0f);
		shadow = variance / (variance + (pixelDepth - shadowMapValues.x)*(pixelDepth - shadowMapValues.x));
		shadow = clamp((shadow - THRESHOLD)/(1 - THRESHOLD), 0, 1.0f);
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

