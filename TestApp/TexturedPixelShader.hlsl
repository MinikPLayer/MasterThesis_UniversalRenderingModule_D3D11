#include "PixelShaderCommon.hlsl"

Texture2D diffuseTexture : register(t0);
SamplerState sampleType : register(s0);

bool useTexture = false;

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 textureColor = diffuseTexture.Sample(sampleType, input.textureUV);
    float3 diffuse = CalculateDiffuseLighting(input);
    return textureColor * float4(diffuse, 1.0f);
    
    //float3 norm = normalize(input.normal);
    //float3 lightDir = normalize(LightPosition - input.fragPosition);
    
    //float diff = max(dot(norm, lightDir), 0.0f);
    //float3 diffuse = lightDir;
    
    //return textureColor * float4(diffuse, 1.0);
}
