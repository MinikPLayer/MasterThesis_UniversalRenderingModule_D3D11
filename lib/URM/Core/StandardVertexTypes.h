#pragma once

#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>

namespace URM::Core {
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

	class VertexPositionNormal {
	public:
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		static std::vector<D3D11_INPUT_ELEMENT_DESC> GetInputLayout() {
			return {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosition, position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPositionNormal, normal), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};
		}
		VertexPositionNormal(float x, float y, float z, float nx, float ny, float nz) : position(x, y, z), normal(nx, ny, nz) {}
	};

	class VertexPositionTexture {
	public:
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texcoord;
		static std::vector<D3D11_INPUT_ELEMENT_DESC> GetInputLayout() {
			return {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosition, position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(VertexPositionTexture, texcoord), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};
		}
		VertexPositionTexture(float x, float y, float z, float u, float v) : position(x, y, z), texcoord(u, v) {}
		VertexPositionTexture() {
			position = { 0.0f, 0.0f, 0.0f };
			texcoord = { 0.0f, 0.0f };
		}
	};

	class VertexPositionNormalTexture {
	public:
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 texcoord;
		static std::vector<D3D11_INPUT_ELEMENT_DESC> GetInputLayout() {
			return {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPositionNormalTexture, position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPositionNormalTexture, normal), D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(VertexPositionNormalTexture, texcoord), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};
		}
		VertexPositionNormalTexture(float x, float y, float z, float nx, float ny, float nz, float u, float v) : position(x, y, z), normal(nx, ny, nz), texcoord(u, v) {}
		VertexPositionNormalTexture() {
			position = { 0.0f, 0.0f, 0.0f };
			normal = { 0.0f, 0.0f, 0.0f };
			texcoord = { 0.0f, 0.0f };
		}
	};
}