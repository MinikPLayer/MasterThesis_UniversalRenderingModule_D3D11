#include "PixelShaderCommon.hlsl"

Texture2D diffuseTexture : register(t0);
SamplerState sampleType : register(s0);

cbuffer CB : register(b1)
{
    Material materialData;
}

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 textureColor = materialData.useAlbedoTexture ? diffuseTexture.Sample(sampleType, input.textureUV) : materialData.albedoColor;
    //return textureColor;
    return float4(0.1, 0.1, 0.1, 1.0f);

}