#include "PixelShaderCommon.hlsl"

Texture2D diffuseTexture : register(t0);
SamplerState sampleType : register(s0);

cbuffer MaterialBuffer : register(b2)
{
    Material material;
}

float4 main(PS_INPUT input) : SV_TARGET {
	float4 textureColor = material.useAlbedoTexture ? diffuseTexture.Sample(sampleType, input.textureUV) : material.albedoColor;
	
	// Transparency is not supported.
    if (textureColor.a < 1.0f)
        discard;
    
    float3 lightColor = float3(0.0, 0.0, 0.0);
	for (int i = 0; i < lightData.activeLightsCount; i++) {
		lightColor += CalculateLighting(input, lightData.lights[i], material.roughnessPowerCoefficient);
	}

	return textureColor * float4(lightColor, 1.0f);
}
