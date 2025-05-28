#include "pch.h"
#include "D3DSampler.h"

namespace URM::Core {
	void D3DSampler::Bind(const D3DCore& core, UINT slot) {
		if (this->mDataChanged) {
			D3D11_SAMPLER_DESC desc = this->mData;

			DX::ThrowIfFailed(
				core.GetDevice()->CreateSamplerState(&desc, this->mSamplerState.GetAddressOf()),
				"Failed to create a Sampler State!"
			);
		}

		core.GetContext()->PSSetSamplers(slot, 1, this->mSamplerState.GetAddressOf());
	}

	void D3DSampler::SetData(const D3DSamplerData& params) {
		this->mData = params;

		this->mDataChanged = true;
	}

	D3DSampler::D3DSampler(const D3DSamplerData& params) {
		this->SetData(params);
	}
}
