#include "SimpleShaderCommonTypes.hlsl"

cbuffer ConstantBuffer : register(b0)
{
    matrix WorldViewProjection;
    matrix WorldMatrix;
    matrix InverseWorldMatrix;
}

struct VS_INPUT
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 textureUV : TEXCOORD;
};

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;
    
    output.pos = mul(float4(input.pos, 1.0f), WorldViewProjection);
    output.fragPosition = mul(float4(input.pos, 1.0f), WorldMatrix).xyz;

    output.textureUV = input.textureUV.xy;
    output.normal = mul(float4(input.normal, 1.0f), InverseWorldMatrix).xyz;
    return output;
}
