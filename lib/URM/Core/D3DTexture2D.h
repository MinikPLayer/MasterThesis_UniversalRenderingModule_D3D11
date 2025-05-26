#pragma once

#include "pch.h"
#include "Utils.h"
#include "D3DCore.h"
#include "D3DSampler.h"

using namespace Microsoft::WRL;

// Based on Assimp's aiTexel
struct Texel2D {
	unsigned int b, g, r, a;
};

struct D3DTexture2DCreationParams {
	D3DSamplerData samplerData = D3DSamplerData();

	UINT mipLevels = 1;
	UINT arraySize = 1;
	UINT samplesCount = 1;
	UINT samplesQuality = 0;
	D3D11_USAGE usage = D3D11_USAGE_DEFAULT;
	DXGI_FORMAT format = DXGI_FORMAT_B8G8R8A8_UNORM;
	UINT bindFlags = D3D11_BIND_SHADER_RESOURCE;
	UINT cpuAccessFlags = 0;
	UINT miscFlags = 0;
};

class D3DTexture2D {
	ComPtr<ID3D11ShaderResourceView> textureView;
	D3DSampler sampler;

	std::string path;
	std::string type;

public:
	std::string GetPath();

	void Bind(D3DCore& core, UINT slot);

	D3DTexture2D(D3DCore& core, std::string name, std::string type, Vector2i size, Texel2D* pixelData, D3DTexture2DCreationParams params = D3DTexture2DCreationParams());
	D3DTexture2D(D3DCore& core, std::string path, std::string type, D3DTexture2DCreationParams params = D3DTexture2DCreationParams());
};