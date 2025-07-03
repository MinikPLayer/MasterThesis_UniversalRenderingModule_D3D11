#pragma once

#include <directxtk/SimpleMath.h>

class Material {
public:
	DirectX::SimpleMath::Color albedo;
	int specularPower = 32;
};