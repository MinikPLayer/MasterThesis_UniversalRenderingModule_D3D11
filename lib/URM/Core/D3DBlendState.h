#pragma once

#include <d3d11.h>
#include "D3DCore.h"

namespace URM::Core {
	struct D3DBlendStateData {
		bool enableBlending = false;
		bool alphaToCoverageEnable = false;
		bool independentBlendEnable = false;
		D3D11_BLEND srcBlend = D3D11_BLEND_ONE;
		D3D11_BLEND destBlend = D3D11_BLEND_ZERO;
		D3D11_BLEND_OP blendOp = D3D11_BLEND_OP_ADD;
		D3D11_BLEND srcBlendAlpha = D3D11_BLEND_ONE;
		D3D11_BLEND destBlendAlpha = D3D11_BLEND_ZERO;
		D3D11_BLEND_OP blendOpAlpha = D3D11_BLEND_OP_ADD;
		UINT8 renderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	};

	class D3DBlendState {
		ComPtr<ID3D11BlendState> blendState;

		D3DBlendStateData mData;
		bool mDataChanged = false;
	public:
		void Bind(const D3DCore& core);

		D3DBlendStateData GetData() const {
			return this->mData;
		}
		void SetData(const D3DBlendStateData& data);

		D3DBlendState(const D3DBlendStateData& data = D3DBlendStateData());
	};
}