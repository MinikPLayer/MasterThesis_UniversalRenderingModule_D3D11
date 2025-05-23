#include "SimpleShaderCommonTypes.hlsl"

cbuffer ConstantBuffer : register(b0)
{
    matrix WorldViewProjection;
    matrix WorldMatrix;
    matrix InverseWorldMatrix;
    //matrix WorldMatrix;
}

struct VS_INPUT
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float4 textureUV : TEXCOORD;
};

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;
    
    output.pos = mul(float4(input.pos, 1.0f), WorldViewProjection);
    output.fragPosition = mul(float4(input.pos, 1.0f), WorldMatrix);
    
    //float4 posWS = mul(float4(input.pos, 1.0f), WorldMatrix);
    //float4 posVS = mul(posWS, ViewMatrix);
    //float4 posPS = mul(posVS, ProjectionMatrix);
    //output.pos = posPS;
    //output.fragPosition = posWS.xyz;
    
    output.textureUV = input.textureUV;
    float4 Normal = mul(float4(input.normal, 1.0f), InverseWorldMatrix);
    output.normal = Normal.xyz;
    return output;
}
