cbuffer ConstantBuffer : register(b0)
{
    matrix WorldViewProjection;
}

struct VS_INPUT
{
    float3 pos : POSITION;
    float4 textureUV : TEXCOORD;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float4 textureUV : TEXCOORD;
};

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;
    output.pos = mul(float4(input.pos, 1.0f), WorldViewProjection);
    output.textureUV = input.textureUV;
    return output;
}
