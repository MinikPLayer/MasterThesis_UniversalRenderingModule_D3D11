struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float2 textureUV : TEXCOORD;
};

Texture2D diffuseTexture : register(t0);
SamplerState sampleType : register(s0);

bool useTexture = false;

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 textureColor = diffuseTexture.Sample(sampleType, input.textureUV);
    return textureColor;
}
