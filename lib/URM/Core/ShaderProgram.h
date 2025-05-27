#pragma once

#include <vector>
#include <string>
#include "D3DCore.h"

#include <d3dcommon.h>

namespace URM::Core {
	enum ShaderStages {
		VERTEX = 0,
		PIXEL = 1,
		GEOMETRY = 2,
	};

	class ShaderProgram {
		ComPtr<ID3D11VertexShader> vertexShader;
		ComPtr<ID3D11PixelShader> pixelShader;

		ComPtr<ID3DBlob> vsSource;
		ComPtr<ID3DBlob> psSource;

		ComPtr<ID3DBlob> LoadShaderBytecode(const std::wstring& fileName);

	public:
		void Bind(D3DCore& core);

		ComPtr<ID3D11VertexShader> GetVertexShader() {
			return this->vertexShader;
		}

		ComPtr<ID3D11PixelShader> GetPixelShader() {
			return this->pixelShader;
		}

		ComPtr<ID3DBlob> GetVertexShaderSource() {
			return this->vsSource;
		}

		ComPtr<ID3DBlob> GetPixelShaderSource() {
			return this->psSource;
		}

		ShaderProgram(D3DCore& core, const std::wstring& vertexPath, const std::wstring& pixelPath);
	};
}