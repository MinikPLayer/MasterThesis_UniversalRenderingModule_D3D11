#include "PixelShaderCommon.hlsl"

Texture2D diffuseTexture : register(t0);
SamplerState sampleType : register(s0);

float4 main(PS_INPUT input) : SV_TARGET {
	float4 textureColor = data.material.useAlbedoTexture ? diffuseTexture.Sample(sampleType, input.textureUV) : data.material.albedoColor;

	float3 lightColor = float3(0.0, 0.0, 0.0);
	for (int i = 0; i < data.activeLightsCount; i++) {
		lightColor += CalculateLighting(input, data.lights[i]);
	}

	return textureColor * float4(lightColor, 1.0f);
}
