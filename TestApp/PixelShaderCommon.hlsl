#include "SimpleShaderCommonTypes.hlsl"

struct Light
{
    float4 color;
    float4 position;
    
    float ambientIntensity;
    float diffuseIntensity;
    float specularIntensity;
};

cbuffer ConstantBuffer : register(b1)
{
    Light light;
    float4 viewPosition;
};

float3 CalculateLighting(PS_INPUT input, float3 lightPosition, float3 lightColor)
{   
    float3 norm = normalize(input.normal);
    float3 lightDir = normalize(lightPosition - input.fragPosition);
    float3 viewDir = normalize(viewPosition.xyz - input.fragPosition);
    float3 reflectDir = reflect(-lightDir, norm);
 
    float ambient = light.ambientIntensity;
    float diff = light.diffuseIntensity * max(dot(norm, lightDir), 0.0f);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    float specular = light.specularIntensity * spec;
    
    return ((diff + ambient + specular) * light.color).xyz;
}

float3 CalculateLighting(PS_INPUT input)
{
    return CalculateLighting(input, light.position.xyz, light.color.xyz);
}