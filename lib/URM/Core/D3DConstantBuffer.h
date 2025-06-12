#pragma once

#include "ID3DBuffer.h"
#include "ShaderProgram.h"

namespace URM::Core {
	class D3DConstantBuffer : public ID3DBuffer {
		ShaderStages mStage;

		D3DConstantBuffer(const D3DCore& core, const D3D11_BUFFER_DESC& desc, const D3D11_SUBRESOURCE_DATA* initData, ShaderStages newStage) : ID3DBuffer(core, desc, initData) {
			this->mStage = newStage;
		}
	public:
		void Bind(D3DCore& core, UINT slot) override {
			switch (mStage) {
				case VERTEX:
					core.GetContext()->VSSetConstantBuffers(slot, 1, this->mBuffer.GetAddressOf());
					break;

				case PIXEL:
					core.GetContext()->PSSetConstantBuffers(slot, 1, this->mBuffer.GetAddressOf());
					break;

				default:
					throw std::runtime_error("Unsupported shader stage");
			}
		}

		template<typename T>
		static D3DConstantBuffer Create(D3DCore& core, ShaderStages shaderStage, UINT cpuAccessFlags = 0, D3D11_USAGE usage = D3D11_USAGE_DEFAULT) {
			D3D11_BUFFER_DESC desc = {};
			desc.Usage = usage;
			desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			desc.ByteWidth = static_cast<UINT>(sizeof(T));
			desc.CPUAccessFlags = cpuAccessFlags;

			return {core, desc, nullptr, shaderStage};
		}
	};
}
