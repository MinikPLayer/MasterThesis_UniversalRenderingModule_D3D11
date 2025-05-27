#pragma once

#include "VertexConcept.h"
#include <memory>
#include <d3d11.h>
#include "ShaderProgram.h"

namespace URM::Core {
	template<VertexTypeConcept VertexType>
	class D3DInputLayout {
		ComPtr<ID3D11InputLayout> inputLayout;

	public:
		void Bind(D3DCore& core) {
			core.GetContext()->IASetInputLayout(this->inputLayout.Get());
		}

		ComPtr<ID3D11InputLayout> get() {
			return this->inputLayout;
		}

		D3DInputLayout(D3DCore& core, ShaderProgram program);
	};

	template<VertexTypeConcept VertexType>
	inline D3DInputLayout<VertexType>::D3DInputLayout(D3DCore& core, ShaderProgram program) {
		auto layout = VertexType::GetInputLayout();

		core.GetDevice()->CreateInputLayout(
			layout.data(),
			(UINT)layout.size(),
			program.GetVertexShaderSource()->GetBufferPointer(),
			program.GetVertexShaderSource()->GetBufferSize(),
			this->inputLayout.GetAddressOf()
		);
	}
}