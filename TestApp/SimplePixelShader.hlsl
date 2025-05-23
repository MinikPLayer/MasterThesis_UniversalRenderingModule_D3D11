#include "PixelShaderCommon.hlsl"

Texture2D diffuseTexture : register(t0);
SamplerState sampleType : register(s0);

bool useTexture = false;

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 diffuse = CalculateLighting(input);
    return 0.5f * float4(diffuse, 1.0f);
}
