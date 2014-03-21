// SkyBox.fx

float4x4 matWVP;
float4x4 matWorld;
texture cubeTex;

float3 centerPos;

samplerCUBE CubeMapS = sampler_state
{
	Texture = <cubeTex>;

	MinFilter = linear;
	MaxAnisotropy = 4;
	MagFilter = linear;
	MipFilter = linear;
	AddressU  = Clamp;
	AddressV  = Clamp;
};


void CubeMappingVS(float3 Pos : POSITION0, 
			out float4 oPos  : POSITION0, 
			out float3 oPosW : TEXCOORD0)
{
	oPos = mul(float4(Pos, 1.0f), matWVP);
	oPosW = (mul(float4(Pos, 1.0f), matWorld)).xyz;
}

float4 CubeMappingPS(float3 posW : TEXCOORD0) : COLOR0
{
	float4 color = float4(0, 0, 0, 1);
	float3 rayDir = posW - centerPos;
	color = texCUBE(CubeMapS, rayDir);
	
	return color;
}

technique CubeMapping
{
	pass P0
	{
		VertexShader = compile vs_3_0 CubeMappingVS();
		PixelShader = compile ps_3_0 CubeMappingPS();		
	}
}
