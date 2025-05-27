#pragma once

#include "D3DCore.h"
#include <d3d11.h>

namespace URM::Core {
	enum class CullModes {
		NONE = D3D11_CULL_NONE,
		FRONT = D3D11_CULL_FRONT,
		BACK = D3D11_CULL_BACK,
	};

	enum class FillModes {
		SOLID = D3D11_FILL_SOLID,
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
		ComPtr<ID3D11RasterizerState> state;
		D3DRasterizerStateData data;

		bool dataChanged = false;

	public:
		void Bind(D3DCore& core);
		void SetData(D3DRasterizerStateData data);
		D3DRasterizerStateData GetData() {
			return this->data;
		}

		D3DRasterizerState(D3DRasterizerStateData data = D3DRasterizerStateData());
	};
}