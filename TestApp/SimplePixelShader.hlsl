#include "PixelShaderCommon.hlsl"

Texture2D diffuseTexture : register(t0);
SamplerState sampleType : register(s0);

bool useTexture = false;

float4 main(PS_INPUT input) : SV_TARGET
{
    return float4(1.0, 0.0, 0.0, 1.0);
}
