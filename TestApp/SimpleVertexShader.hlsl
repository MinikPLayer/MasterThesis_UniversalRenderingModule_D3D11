cbuffer ConstantBuffer : register(b0)
{
    matrix WorldViewProjection;
}

struct VS_INPUT
{
    float3 Pos : POSITION;
    float4 TextureUV : TEXCOORD;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
};

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;
    output.Pos = mul(float4(input.Pos, 1.0f), WorldViewProjection);
    output.Color = float4(input.TextureUV.x, input.TextureUV.y, 0.0f, 1.0f);
    return output;
}
