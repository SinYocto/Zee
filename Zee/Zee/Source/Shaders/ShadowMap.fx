// ShadowMap.fx

float4x4 matWVP;
float4x4 matWorld;

float3 lightPos;

struct VS_IN
{
	float3 pos : POSITION0;
};

struct VS_OUT
{
	float4 pos : POSITION0;
	float3 posW : TEXCOORD0;
};

VS_OUT DepthVS(VS_IN vIn)
{
	VS_OUT vOut;

	vOut.pos = mul(float4(vIn.pos, 1.0f), matWVP);
	vOut.posW = (mul(float4(vIn.pos, 1.0f), matWorld)).xyz;

	return vOut;
}

float4 DepthPS(float4 posW : TEXCOORD0) : COLOR0		
{
	float depth = length(lightPos - posW.xyz);
	return float4(depth, depth*depth, 0, 1);
	//return float4(0.5f, 0.5f, 0, 1);
}

technique Depth
{
	pass P0
	{
		VertexShader = compile vs_3_0 DepthVS();
		PixelShader = compile ps_3_0 DepthPS();		
	}
}
