#include "SimpleShaderCommonTypes.hlsl"

cbuffer CB : register(b1)
{
    PixelUniformData data;
}

struct PBRMaterial
{
    float4 albedo; // 16B
    bool useAlbedoTexture; // 4B
    float metallic; // 4B
    float roughness; // 4B
    float ao; // 4B
};

cbuffer MaterialCB : register(b2)
{
    PBRMaterial material;
}

struct PBRLight
{
    float3 color;
    float3 position;
};

struct PBRLightUniformData
{
    int activeLightsCount;
    PBRLight lights[MAX_LIGHTS_COUNT];
};

cbuffer LightUniformData : register(b3)
{
    PBRLightUniformData lightData;
}

static const float PI = 3.14159265359;

Texture2D diffuseTexture : register(t0);
SamplerState sampleType : register(s0);

float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    
    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = r * r / 8.0;
    
    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    
    return num / denom;
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    
    float ggx1 = GeometrySchlickGGX(NdotV, roughness);
    float ggx2 = GeometrySchlickGGX(NdotL, roughness);
    
    return ggx1 * ggx2;
}

float3 FresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float3 CalculatePBR(PS_INPUT input, float3 albedo)
{
    float3 N = normalize(input.normal);
    float3 V = normalize(data.viewPosition.xyz - input.fragPosition);
    
    float3 F0 = lerp(float3(0.04, 0.04, 0.04), albedo, material.metallic);
    
    float3 Lo = float3(0.0, 0.0, 0.0);
    
    [unroll(MAX_LIGHTS_COUNT)]
    for (int i = 0; i < MAX_LIGHTS_COUNT; i++)
    {
        if(i >= lightData.activeLightsCount)
            break;
        
        float3 L = normalize(lightData.lights[i].position - input.fragPosition);
        float3 H = normalize(V + L);
        float distance = length(lightData.lights[i].position - input.fragPosition);
        //float attenuation = 1.0 / (distance * distance);
        float attenuation = 1.0;
        float3 radiance = lightData.lights[i].color * attenuation;

        float NDF = DistributionGGX(N, H, material.roughness);
        float G = GeometrySmith(N, V, L, material.roughness);
        float3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);
        
        float3 kS = F;
        float3 kD = float3(1.0, 1.0, 1.0) - kS;
        kD *= 1.0 - material.metallic;

        float3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        float3 specular = numerator / denominator;

        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }
    
    float3 ambient = float3(0.03, 0.03, 0.03) * albedo * material.ao;
    float3 color = ambient + Lo;
    
    color = color / (color + float3(1.0, 1.0, 1.0));
    
    float gammaCorrection = 1.0 / 2.2;
    color = pow(abs(color), float3(gammaCorrection, gammaCorrection, gammaCorrection)); // Gamma correction

    return color;
}

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 textureColor = material.useAlbedoTexture ? diffuseTexture.Sample(sampleType, input.textureUV) : material.albedo;
    float3 lightColor = CalculatePBR(input, textureColor.rgb);
    return float4(textureColor.rgb * lightColor, 1.0f);
}
