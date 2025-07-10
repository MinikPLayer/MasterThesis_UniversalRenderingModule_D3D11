#include "../lib/URM/Engine/PixelShaderCommon.hlsl"

struct ScreenData
{
    float2 screenSize;
    bool calculateLighting;
};

cbuffer ScreenDataCB : register(b2)
{
    ScreenData screenData;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    float2 pixelPosition = input.pos.xy / screenData.screenSize;
    float4 pixelColor = float4(pixelPosition, 0.0f, 1.0f);
    
    float3 lightColor = float3(0.0, 0.0, 0.0);
    if (screenData.calculateLighting)
    {
        for (int i = 0; i < lightData.activeLightsCount; i++)
        {
            lightColor += CalculateLighting(input, lightData.lights[i], 32);
        }
    }
    else
    {
        lightColor = float3(1.0, 1.0, 1.0);
    }

    return pixelColor * float4(lightColor, 1.0f);
}