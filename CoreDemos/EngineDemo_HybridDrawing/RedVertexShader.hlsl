struct PS_INPUT
{
    float4 pos : SV_Position;
};

struct VS_INPUT
{
    float3 pos : POSITION;
};

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;

    output.pos = float4(input.pos, 1.0f);
	
    return output;
}
