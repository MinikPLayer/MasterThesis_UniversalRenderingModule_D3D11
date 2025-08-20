#include "../../lib/URM/Engine/SimpleShaderCommonTypes.hlsl"

float4 main(PS_INPUT input) : SV_TARGET
{
    return float4(input.normal, 1.0f);
}