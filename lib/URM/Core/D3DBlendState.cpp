#include "pch.h"
#include "D3DBlendState.h"

void URM::Core::D3DBlendState::Bind(const D3DCore& core) {
	if (this->mDataChanged) {
		D3D11_BLEND_DESC desc = {};
		desc.AlphaToCoverageEnable = this->mData.alphaToCoverageEnable;
		desc.IndependentBlendEnable = this->mData.independentBlendEnable;
		for (int i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i) {
			desc.RenderTarget[i].BlendEnable = this->mData.enableBlending;
			desc.RenderTarget[i].SrcBlend = this->mData.srcBlend;
			desc.RenderTarget[i].DestBlend = this->mData.destBlend;
			desc.RenderTarget[i].BlendOp = this->mData.blendOp;
			desc.RenderTarget[i].SrcBlendAlpha = this->mData.srcBlendAlpha;
			desc.RenderTarget[i].DestBlendAlpha = this->mData.destBlendAlpha;
			desc.RenderTarget[i].BlendOpAlpha = this->mData.blendOpAlpha;
			desc.RenderTarget[i].RenderTargetWriteMask = this->mData.renderTargetWriteMask;
		}
		ID3D11Device1* device = core.GetDevice();
		ID3D11DeviceContext1* context = core.GetContext();
		DX::ThrowIfFailed(device->CreateBlendState(&desc, this->blendState.ReleaseAndGetAddressOf()));
		this->mDataChanged = false;
	}

	core.GetContext()->OMSetBlendState(this->blendState.Get(), nullptr, 0xFFFFFFFF);
}

void URM::Core::D3DBlendState::SetData(const D3DBlendStateData& data) {
	this->mData = data;
	this->mDataChanged = true;
}

URM::Core::D3DBlendState::D3DBlendState(const D3DBlendStateData& data) {
	this->SetData(data);
}