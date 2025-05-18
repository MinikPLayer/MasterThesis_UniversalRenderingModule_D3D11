#include "pch.h"
#include "D3DRasterizerState.h"

void D3DRasterizerState::Bind(D3DCore& core) {
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
	ComPtr<ID3D11RasterizerState> rasterizerState;
	DX::ThrowIfFailed(
		core.GetDevice()->CreateRasterizerState(&desc, rasterizerState.GetAddressOf()),
		"Failed to create a Rasterizer State!"
	);

	core.GetContext()->RSSetState(rasterizerState.Get());
}

void D3DRasterizerState::SetData(D3DRasterizerStateData newData) {
	this->data = newData;
}

D3DRasterizerState::D3DRasterizerState(D3DRasterizerStateData data) {
	this->SetData(data);
}
