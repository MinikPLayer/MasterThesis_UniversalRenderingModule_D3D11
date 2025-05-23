#include "pch.h"
#include "D3DRasterizerState.h"

void D3DRasterizerState::Bind(D3DCore& core) {
	if (this->dataChanged) {
		if (this->state.Get() != nullptr) {
			this->state->Release();
		}

		D3D11_RASTERIZER_DESC desc = {};
		ZeroMemory(&desc, sizeof(D3D11_RASTERIZER_DESC));
		desc.FillMode = static_cast<D3D11_FILL_MODE>(this->data.fillMode);
		desc.CullMode = static_cast<D3D11_CULL_MODE>(this->data.cullMode);
		desc.FrontCounterClockwise = this->data.frontCounterClockwise;
		desc.DepthBias = this->data.depthBias;
		desc.DepthBiasClamp = this->data.depthBiasClamp;
		desc.SlopeScaledDepthBias = this->data.slopeScaledDepthBias;
		desc.DepthClipEnable = this->data.enableDepthClip;
		desc.ScissorEnable = this->data.enableScissor;
		desc.MultisampleEnable = this->data.enableMultisampling;
		desc.AntialiasedLineEnable = this->data.enableLineAA;
		DX::ThrowIfFailed(
			core.GetDevice()->CreateRasterizerState(&desc, state.GetAddressOf()),
			"Failed to create a Rasterizer State!"
		);

		this->dataChanged = false;
	}

	core.GetContext()->RSSetState(state.Get());
}

void D3DRasterizerState::SetData(D3DRasterizerStateData newData) {
	this->data = newData;

	this->dataChanged = true;
}

D3DRasterizerState::D3DRasterizerState(D3DRasterizerStateData data) {
	this->SetData(data);
}
