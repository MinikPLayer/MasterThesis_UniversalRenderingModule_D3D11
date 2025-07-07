#pragma once

#include "VertexConcept.h"
#include <d3d11.h>
#include "ShaderPipeline.h"
#include "ModelLoader.h"

namespace URM::Core {
	template<VertexTypeConcept VertexType>
	class D3DInputLayout {
		ComPtr<ID3D11InputLayout> mInputLayout;
	public:
		void Bind(const D3DCore& core) const {
			core.GetContext()->IASetInputLayout(this->mInputLayout.Get());
		}

		ComPtr<ID3D11InputLayout> Get() {
			return this->mInputLayout;
		}

		D3DInputLayout(const D3DCore& core, VertexShader& program);
	};

	template<VertexTypeConcept VertexType>
	D3DInputLayout<VertexType>::D3DInputLayout(const D3DCore& core, VertexShader& vertexProgram) {
		auto layout = VertexType::GetInputLayout();

		core.GetDevice()->CreateInputLayout(
			layout.data(),
			static_cast<UINT>(layout.size()),
			vertexProgram.GetBytecode()->GetBufferPointer(),
			vertexProgram.GetBytecode()->GetBufferSize(),
			this->mInputLayout.GetAddressOf()
		);
	}

	using ModelLoaderLayout = D3DInputLayout<ModelLoaderVertexType>;
}
