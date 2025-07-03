#include "PixelShaderCommon.hlsl"

static const int MAX_LIGHTS_COUNT = 32;

struct Light
{
    float3 color; // 16B
    float3 position; // 16B

    float ambientIntensity; // 4B
    float diffuseIntensity; // 4B
    float specularIntensity; // 4B
};

struct PixelLightingUniformData
{
    float4 viewPosition; // 16B
    Material material; // 16B
    int activeLightsCount;
    Light lights[MAX_LIGHTS_COUNT]; // 8 * 48B
};

cbuffer CB : register(b1)
{
    PixelLightingUniformData data;
}

float3 CalculateLighting(PS_INPUT input, Light light)
{
    float3 norm = normalize(input.normal);
    float3 lightDir = normalize(light.position - input.fragPosition);
    float3 viewDir = normalize(data.viewPosition.xyz - input.fragPosition);
    float3 reflectDir = reflect(-lightDir, norm);

    float ambient = light.ambientIntensity;
    float diff = light.diffuseIntensity * max(dot(norm, lightDir), 0.0f);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), data.material.roughnessPowerCoefficient);
    float specular = light.specularIntensity * spec;

	//return ((diff + ambient + specular) * light.color).xyz;
    return (ambient + diff + specular) * light.color;

}

float4 main(PS_INPUT input) : SV_TARGET {
	float3 lightColor = float3(0.0, 0.0, 0.0);
	for (int i = 0; i < data.activeLightsCount; i++) {
		lightColor += CalculateLighting(input, data.lights[i]);
	}

    //return textureColor * float4(lightColor, 1.0f);
    return float4(lightColor, 1.0f);

}
