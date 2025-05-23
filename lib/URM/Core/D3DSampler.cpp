#include "pch.h"
#include "D3DSampler.h"

void D3DSampler::Bind(D3DCore& core, UINT slot) {
	if (this->dataChanged) {
		D3D11_SAMPLER_DESC desc = this->data;

		DX::ThrowIfFailed(
			core.GetDevice()->CreateSamplerState(&desc, this->samplerState.GetAddressOf()),
			"Failed to create a Sampler State!"
		);
	}

	core.GetContext()->PSSetSamplers(slot, 1, this->samplerState.GetAddressOf());
}

void D3DSampler::SetData(D3DSamplerData params) {
	this->data = params;

	this->dataChanged = true;
}

D3DSampler::D3DSampler(D3DSamplerData params) {
	this->SetData(params);
}
