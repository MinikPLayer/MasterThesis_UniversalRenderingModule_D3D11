#include "SimpleShaderCommonTypes.hlsl"

static const int MAX_LIGHTS_COUNT = 8;

struct Light
{
    float3 color; // 16B
    float3 position; // 16B
    
    float ambientIntensity; // 4B
    float diffuseIntensity; // 4B
    float specularIntensity; // 4B
};

struct Material
{
    bool useAlbedoTexture; // 4B
};

struct PixelUniformData
{
    float4 viewPosition; // 16B
    Material material; // 16B
    int activeLightsCount;
    Light lights[MAX_LIGHTS_COUNT]; // 8 * 48B
};

cbuffer CB : register(b1)
{
    PixelUniformData data;
}

float3 CalculateLighting(PS_INPUT input, Light light)
{   
    float3 norm = normalize(input.normal);
    float3 lightDir = normalize(light.position - input.fragPosition);
    float3 viewDir = normalize(data.viewPosition.xyz - input.fragPosition);
    float3 reflectDir = reflect(-lightDir, norm);
 
    float ambient = light.ambientIntensity;
    float diff = light.diffuseIntensity * max(dot(norm, lightDir), 0.0f);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    float specular = light.specularIntensity * spec;
    
    //return ((diff + ambient + specular) * light.color).xyz;
    return (ambient + diff + specular) * light.color;

}