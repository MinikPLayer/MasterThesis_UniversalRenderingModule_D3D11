#include "PixelShaderCommon.hlsl"

Texture2D diffuseTexture : register(t0);
SamplerState sampleType : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 textureColor = material.useAlbedoTexture ? diffuseTexture.Sample(sampleType, input.textureUV) : float4(1.0, 0.0, 1.0, 1.0);
    float3 diffuse = CalculateLighting(input);
    return textureColor * float4(diffuse, 1.0f);
}
