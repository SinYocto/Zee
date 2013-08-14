// Flat.fx

float4x4 matWVP;
float4x4 matUVTransform;

float4 mtlColor;

bool useColorTex;
texture colorTex;

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

VS_OUT FlatVS(VS_IN vIn)
{
	VS_OUT vOut;

	vOut.pos = mul(float4(vIn.pos, 1.0f), matWVP);
	vOut.tex = (mul(float4(vIn.tex, 0, 1.0f), matUVTransform)).xy;

	return vOut;
}

float4 FlatPS(float2 tex : TEXCOORD0) : COLOR0
{
	float4 oColor = mtlColor;

	if(useColorTex)
	{
		oColor *= tex2D(ColorS, tex);
	}

	return oColor;
}

technique Flat
{
	pass P0
	{
		VertexShader = compile vs_3_0 FlatVS();
		PixelShader = compile ps_3_0 FlatPS();		
	}
}
