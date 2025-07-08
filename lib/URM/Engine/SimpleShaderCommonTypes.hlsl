static const int MAX_LIGHTS_COUNT = 64;

struct PS_INPUT {
	float4 pos : SV_Position;
	float3 normal : NORMAL;
	float2 textureUV : TEXCOORD;
	float3 fragPosition : POSITION0;
};

struct Light
{
    float3 color; // 16B
    float3 position; // 16B

    float ambientIntensity; // 4B
    float diffuseIntensity; // 4B
    float specularIntensity; // 4B
    
    float attenuationExponent; // 4B
    float pbrIntensity; // 4B
};

struct Material
{
    bool useAlbedoTexture; // 4B
    int roughnessPowerCoefficient; // 4B
    float4 albedoColor; // 16B
};

struct PixelUniformData
{
    float4 viewPosition; // 16B
};

struct LightUniformData
{
    int activeLightsCount;
    Light lights[MAX_LIGHTS_COUNT]; // 8 * 48B
};