#pragma once

#include <d3d11.h>
#include <wrl.h>

#include "D3DCore.h"

using namespace Microsoft::WRL;

namespace URM::Core {
	struct D3DSamplerData : public D3D11_SAMPLER_DESC {
		D3DSamplerData() {
			this->Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
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
		ComPtr<ID3D11SamplerState> samplerState;
		D3DSamplerData data;

		bool dataChanged = false;
	public:
		void Bind(D3DCore& core, UINT slot);
		D3DSamplerData GetData() {
			return this->data;
		}

		void SetData(D3DSamplerData params);
		D3DSampler(D3DSamplerData params = D3DSamplerData());
	};
}