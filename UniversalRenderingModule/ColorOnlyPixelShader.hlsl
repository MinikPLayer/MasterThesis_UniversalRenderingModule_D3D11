#include "../lib/URM/Engine/PixelShaderCommon.hlsl"

Texture2D diffuseTexture : register(t0);
SamplerState sampleType : register(s0);

bool useTexture = false;

cbuffer MaterialBuffer : register(b2)
{
    Material material;
}

float4 main(PS_INPUT input) : SV_TARGET {
	float3 lightColor = float3(0.0, 0.0, 0.0);
	for (int i = 0; i < lightData.activeLightsCount; i++) {
		lightColor += CalculateLighting(input, lightData.lights[i], material.roughnessPowerCoefficient);
	}

	return 0.5f * float4(lightColor, 1.0f);
}
