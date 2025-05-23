#include "PixelShaderCommon.hlsl"

Texture2D diffuseTexture : register(t0);
SamplerState sampleType : register(s0);

bool useTexture = false;

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 diffuse = CalculateDiffuseLighting(input);
    return color * float4(diffuse, 1.0f);
}
