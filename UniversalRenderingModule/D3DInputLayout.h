#pragma once

#include "VertexConcept.h"
#include <memory>
#include <d3d11.h>
#include "ShaderProgram.h"

template<VertexTypeConcept VertexType>
class D3DInputLayout {
	ComPtr<ID3D11InputLayout> inputLayout;

public:
	// TODO: Add function to bind to the context

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
		layout.size(),
		program.GetVertexShaderSource()->GetBufferPointer(),
		program.GetVertexShaderSource()->GetBufferSize(),
		this->inputLayout.GetAddressOf()
	);
}
