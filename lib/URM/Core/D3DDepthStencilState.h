#pragma once

#include <d3d11.h>
#include "D3DCore.h"

namespace URM::Core {
	struct D3DDepthStencilStateData {
		bool depthEnable = true;
		D3D11_DEPTH_WRITE_MASK depthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		D3D11_COMPARISON_FUNC depthFunc = D3D11_COMPARISON_LESS_EQUAL;
		bool stencilEnable = false;
		UINT8 stencilReadMask = 0xFF;
		UINT8 stencilWriteMask = 0xFF;
		D3D11_DEPTH_STENCILOP_DESC frontFaceOp = {};
		D3D11_DEPTH_STENCILOP_DESC backFaceOp = {};
	};

	class D3DDepthStencilState {
		ComPtr<ID3D11DepthStencilState> depthStencilState;

		D3DDepthStencilStateData mData;
		bool mDataChanged = false;
	public:
		void Bind(const D3DCore& core);

		D3DDepthStencilStateData GetData() const {
			return this->mData;
		}

		void SetData(const D3DDepthStencilStateData& data);

		D3DDepthStencilState(const D3DDepthStencilStateData& data = D3DDepthStencilStateData());

	};
};