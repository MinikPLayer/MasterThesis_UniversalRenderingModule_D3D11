#pragma once

#include <string>
#include "D3DCore.h"

#include <d3dcommon.h>

namespace URM::Core {
	enum ShaderStages {
		VERTEX   = 0,
		PIXEL    = 1,
		GEOMETRY = 2,
	};

	class ShaderProgram {
		ComPtr<ID3D11VertexShader> mVertexShader;
		ComPtr<ID3D11PixelShader> mPixelShader;

		ComPtr<ID3DBlob> mVertexSource;
		ComPtr<ID3DBlob> mPixelSource;

		static ComPtr<ID3DBlob> LoadShaderBytecode(const std::wstring& fileName);
	public:
		void Bind(const D3DCore& core) const;

		ComPtr<ID3D11VertexShader> GetVertexShader() {
			return this->mVertexShader;
		}

		ComPtr<ID3D11PixelShader> GetPixelShader() {
			return this->mPixelShader;
		}

		ComPtr<ID3DBlob> GetVertexShaderSource() {
			return this->mVertexSource;
		}

		ComPtr<ID3DBlob> GetPixelShaderSource() {
			return this->mPixelSource;
		}

		ShaderProgram(const D3DCore& core, const std::wstring& vertexPath, const std::wstring& pixelPath);
	};
}
