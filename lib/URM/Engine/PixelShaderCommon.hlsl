#include "SimpleShaderCommonTypes.hlsl"

struct LightUniformData
{
    int activeLightsCount;
    Light lights[MAX_LIGHTS_COUNT]; // 8 * 48B
};

cbuffer CB : register(b1) {
	PixelUniformData data;
}

cbuffer MaterialBuffer : register(b2)
{
	Material material;
}

cbuffer LightsBuffer : register(b3)
{
	LightUniformData lightData;
}

float3 CalculateLighting(PS_INPUT input, Light light) {
	float3 norm = normalize(input.normal);
	float3 lightDir = normalize(light.position - input.fragPosition);
	float3 viewDir = normalize(data.viewPosition.xyz - input.fragPosition);
	float3 reflectDir = reflect(-lightDir, norm);

	float ambient = light.ambientIntensity;
	float diff = light.diffuseIntensity * max(dot(norm, lightDir), 0.0f);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.roughnessPowerCoefficient);
	float specular = light.specularIntensity * spec;

	//return ((diff + ambient + specular) * light.color).xyz;
	return (ambient + diff + specular) * light.color;
}
