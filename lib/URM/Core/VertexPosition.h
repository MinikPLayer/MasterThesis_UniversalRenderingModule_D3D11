#pragma once

#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>

class VertexPosition {
public:
	DirectX::XMFLOAT3 position;

	static std::vector<D3D11_INPUT_ELEMENT_DESC> GetInputLayout() {
		return {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosition, position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
	}

	VertexPosition(float x, float y, float z) : position(x, y, z) {}
};

class VertexPositionColor {
public:
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT4 color;

	static std::vector<D3D11_INPUT_ELEMENT_DESC> GetInputLayout() {
		return {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosition, position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(VertexPositionColor, color), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
	}

	VertexPositionColor(float x, float y, float z, float r, float g, float b) : position(x, y, z), color(r, g, b, 1.0f) {}
};