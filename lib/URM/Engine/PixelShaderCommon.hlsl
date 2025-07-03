#include "SimpleShaderCommonTypes.hlsl"

struct Material {
	bool useAlbedoTexture; // 4B
    int roughnessPowerCoefficient; // 4B
    float4 albedoColor; // 16B
};