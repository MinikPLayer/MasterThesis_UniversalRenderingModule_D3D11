#include "SimpleShaderCommonTypes.hlsl"

cbuffer ConstantBuffer : register(b1)
{
    float4 color;
};

static const float3 DefaultLightPosition = float3(0.0f, 2.0f, 4.0f);
static const float3 DefaultLightColor = float3(1.0f, 1.0f, 1.0f);

float3 CalculateDiffuseLighting(PS_INPUT input, float3 lightPosition, float3 lightColor)
{
    float3 norm = normalize(input.normal);
    float3 lightDir = normalize(lightPosition - input.fragPosition);
    
    float diff = max(dot(norm, lightDir), 0.0f);
    float3 diffuse = lightColor * diff;
    
    return diffuse;
}

float3 CalculateDiffuseLighting(PS_INPUT input)
{
    return CalculateDiffuseLighting(input, DefaultLightPosition, DefaultLightColor);
}