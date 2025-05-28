#pragma once

#include "pch.h"
#include "Utils.h"
#include "D3DCore.h"
#include "D3DSampler.h"

using namespace Microsoft::WRL;

namespace URM::Core {
	// Based on Assimp's aiTexel
	struct Texel2D {
		unsigned int b, g, r, a;
	};

	struct D3DTexture2DCreationParams {
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
		ComPtr<ID3D11ShaderResourceView> mTextureView;

		std::string mPath;
		std::string mType;
	public:
		std::string GetPath();

		void Bind(const D3DCore& core, UINT slot);

		D3DTexture2D(const D3DCore& core, const std::string& name, const std::string& type, Size2i size, const Texel2D* pixelData, const D3DTexture2DCreationParams& params = D3DTexture2DCreationParams());
		D3DTexture2D(const D3DCore& core, const std::string& path, const std::string& type, const D3DTexture2DCreationParams& params = D3DTexture2DCreationParams());
	};
}
