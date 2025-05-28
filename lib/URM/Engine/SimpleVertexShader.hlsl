#include "SimpleShaderCommonTypes.hlsl"

struct WorldData {
	matrix WorldViewProjection;
	matrix WorldMatrix;
	matrix InverseWorldMatrix;
};

struct VS_INPUT {
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 textureUV : TEXCOORD;
};

cbuffer CB : register(b0) {
	WorldData data;
};

PS_INPUT main(VS_INPUT input) {
	PS_INPUT output;

	output.pos = mul(float4(input.pos, 1.0f), data.WorldViewProjection);
	output.fragPosition = mul(float4(input.pos, 1.0f), data.WorldMatrix).xyz;

	output.textureUV = input.textureUV.xy;
	output.normal = mul(float4(input.normal, 1.0f), data.InverseWorldMatrix).xyz;
	return output;
}
