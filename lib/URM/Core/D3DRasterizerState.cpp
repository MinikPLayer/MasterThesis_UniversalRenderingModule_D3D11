#include "pch.h"
#include "D3DRasterizerState.h"

namespace URM::Core {
	void D3DRasterizerState::Bind(const D3DCore& core) {
		if (this->mDataChanged) {
			if (this->mState.Get() != nullptr) {
				this->mState->Release();
			}

			D3D11_RASTERIZER_DESC desc = {};
			ZeroMemory(&desc, sizeof(D3D11_RASTERIZER_DESC));
			desc.FillMode = static_cast<D3D11_FILL_MODE>(this->mData.fillMode);
			desc.CullMode = static_cast<D3D11_CULL_MODE>(this->mData.cullMode);
			desc.FrontCounterClockwise = this->mData.frontCounterClockwise;
			desc.DepthBias = this->mData.depthBias;
			desc.DepthBiasClamp = this->mData.depthBiasClamp;
			desc.SlopeScaledDepthBias = this->mData.slopeScaledDepthBias;
			desc.DepthClipEnable = this->mData.enableDepthClip;
			desc.ScissorEnable = this->mData.enableScissor;
			desc.MultisampleEnable = this->mData.enableMultisampling;
			desc.AntialiasedLineEnable = this->mData.enableLineAA;
			DX::ThrowIfFailed(
				core.GetDevice()->CreateRasterizerState(&desc, mState.GetAddressOf()),
				"Failed to create a Rasterizer State!"
			);

			this->mDataChanged = false;
		}

		core.GetContext()->RSSetState(mState.Get());
	}

	void D3DRasterizerState::SetData(const D3DRasterizerStateData& newData) {
		this->mData = newData;

		this->mDataChanged = true;
	}

	D3DRasterizerState::D3DRasterizerState(const D3DRasterizerStateData& data) {
		this->SetData(data);
	}
}
