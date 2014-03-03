// GaussianBlur.fx

int sourceTexSize;
texture	sourceTex;

sampler SourceTexS = sampler_state
{
	Texture = <sourceTex>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
	AddressU  = Clamp;
    AddressV  = Clamp;
};

float blurWeights5[5] = 
{
    0.054489,
    0.244201,
    0.402620,
    0.244201,
    0.054489
};

float2 pixelKernelH5[5] =
{
    { -2, 0 },
    { -1, 0 },
    {  0, 0 },
    {  1, 0 },
    {  2, 0 }
};


float2 pixelKernelV5[5] =
{
    { 0, -2 },
    { 0, -1 },
    { 0,  0 },
    { 0,  1 },
    { 0,  2 }
};

float blurWeights[13] = 
{
    0.002216,
    0.008764,
    0.026995,
    0.064759,
    0.120985,
    0.176033,
    0.199471,
    0.176033,
    0.120985,
    0.064759,
    0.026995,
    0.008764,
    0.002216,
};

float2 pixelKernelH[13] =
{
    { -6, 0 },
    { -5, 0 },
    { -4, 0 },
    { -3, 0 },
    { -2, 0 },
    { -1, 0 },
    {  0, 0 },
    {  1, 0 },
    {  2, 0 },
    {  3, 0 },
    {  4, 0 },
    {  5, 0 },
    {  6, 0 },
};

float2 pixelKernelV[13] =
{
    { 0, -6 },
    { 0, -5 },
    { 0, -4 },
    { 0, -3 },
    { 0, -2 },
    { 0, -1 },
    { 0,  0 },
    { 0,  1 },
    { 0,  2 },
    { 0,  3 },
    { 0,  4 },
    { 0,  5 },
    { 0,  6 },
};


float4 GaussianBlurHPS(float2 tex : TEXCOORD0) : COLOR0
{
    float4 color = 0;

    for(int i = 0; i < 13; i++)
    {    
        color += tex2D(SourceTexS, tex + pixelKernelH[i].xy/sourceTexSize) * blurWeights[i];
    }

    return color;
}


float4 GaussianBlurVPS(float2 tex : TEXCOORD0) : COLOR0
{
    float4 color = 0;

    for (int i = 0; i < 13; i++)
    {    
        color += tex2D(SourceTexS, tex + pixelKernelV[i].xy/sourceTexSize) * blurWeights[i];
    }

    return color;
}

technique GaussianBlurH
{
    pass p0
    {
        VertexShader = null;
        PixelShader = compile ps_3_0 GaussianBlurHPS();
    }
}

technique GaussianBlurV
{
    pass p0
    {
        VertexShader = null;
        PixelShader = compile ps_3_0 GaussianBlurVPS();
    }
}