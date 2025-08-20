struct PS_INPUT
{
	float4 pos : SV_Position;
	float4 color : COLOR;
};


struct WorldData
{
	matrix WorldViewProjection;
	matrix WorldMatrix;
	matrix InverseWorldMatrix;
};

struct VS_INPUT
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 textureUV : TEXCOORD;
};

cbuffer CB : register(b0)
{
	WorldData data;
};

PS_INPUT main(VS_INPUT input)
{
	PS_INPUT output;

	output.pos = mul(float4(input.pos, 1.0f), data.WorldViewProjection);
	output.color = float4(input.normal, 1.0f);
	
	return output;
}
