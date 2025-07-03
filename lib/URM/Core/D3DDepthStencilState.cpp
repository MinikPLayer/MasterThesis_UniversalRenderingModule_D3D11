#include "pch.h"
#include "D3DDepthStencilState.h"

void URM::Core::D3DDepthStencilState::Bind(const D3DCore& core) {
	if (this->mDataChanged) {
		D3D11_DEPTH_STENCIL_DESC desc = {};
		desc.DepthEnable = this->mData.depthEnable;
		desc.DepthWriteMask = this->mData.depthWriteMask;
		desc.DepthFunc = this->mData.depthFunc;
		desc.StencilEnable = this->mData.stencilEnable;
		desc.StencilReadMask = this->mData.stencilReadMask;
		desc.StencilWriteMask = this->mData.stencilWriteMask;
		desc.FrontFace = this->mData.frontFaceOp;
		desc.BackFace = this->mData.backFaceOp;
		ID3D11Device1* device = core.GetDevice();
		ID3D11DeviceContext1* context = core.GetContext();
		DX::ThrowIfFailed(device->CreateDepthStencilState(&desc, this->depthStencilState.ReleaseAndGetAddressOf()));
		this->mDataChanged = false;
	}

	core.GetContext()->OMSetDepthStencilState(this->depthStencilState.Get(), 0);
}

void URM::Core::D3DDepthStencilState::SetData(const D3DDepthStencilStateData& data) {
	this->mData = data;
	this->mDataChanged = true;
}

URM::Core::D3DDepthStencilState::D3DDepthStencilState(const D3DDepthStencilStateData& data) {
	this->SetData(data);
}