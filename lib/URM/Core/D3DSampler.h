#pragma once

#include <d3d11.h>
#include <wrl.h>

#include "D3DCore.h"

using namespace Microsoft::WRL;

namespace URM::Core {
	struct D3DSamplerData : D3D11_SAMPLER_DESC {
		D3DSamplerData() {
			this->Filter = D3D11_FILTER_ANISOTROPIC;
			this->AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			this->AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			this->AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			this->ComparisonFunc = D3D11_COMPARISON_NEVER;
			this->MinLOD = 0;
			this->MaxLOD = D3D11_FLOAT32_MAX;
			this->MipLODBias = 0;
			this->MaxAnisotropy = 16;
		}
	};

	class D3DSampler {
		ComPtr<ID3D11SamplerState> mSamplerState;
		D3DSamplerData mData;

		bool mDataChanged = false;
	public:
		void Bind(const D3DCore& core, UINT slot);
		D3DSamplerData GetData() const {
			return this->mData;
		}

		void SetData(const D3DSamplerData& params);
		D3DSampler(const D3DSamplerData& params = D3DSamplerData());
	};
}
