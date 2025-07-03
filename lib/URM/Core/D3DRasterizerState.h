#pragma once

#include "D3DCore.h"
#include <d3d11.h>

namespace URM::Core {
	// Back and front inverted because of RH coordinate system, while D3D11 assumes LH.
	enum class CullModes {
		NONE  = D3D11_CULL_NONE,
		FRONT = D3D11_CULL_FRONT,
		BACK  = D3D11_CULL_BACK,
	};

	enum class FillModes {
		SOLID     = D3D11_FILL_SOLID,
		WIREFRAME = D3D11_FILL_WIREFRAME,
	};

	struct D3DRasterizerStateData {
		CullModes cullMode = CullModes::BACK;
		FillModes fillMode = FillModes::SOLID;

		bool frontCounterClockwise = false;
		int depthBias = 0;
		float depthBiasClamp = 0.0f;
		float slopeScaledDepthBias = 0.0f;

		bool enableDepthClip = true;
		bool enableScissor = false;
		bool enableMultisampling = false;
		bool enableLineAA = false;

		D3DRasterizerStateData() {}
	};

	class D3DRasterizerState {
		ComPtr<ID3D11RasterizerState> mState;
		D3DRasterizerStateData mData;

		bool mDataChanged = false;
	public:
		void Bind(const D3DCore& core);
		void SetData(const D3DRasterizerStateData& data);
		D3DRasterizerStateData GetData() const {
			return this->mData;
		}

		D3DRasterizerState(const D3DRasterizerStateData& data = D3DRasterizerStateData());
	};
}
