#include "pch.h"
#include "ShaderPipeline.h"

#include <d3dcompiler.h>

#pragma comment(lib, "d3dcompiler.lib")

namespace URM::Core {
	ComPtr<ID3DBlob> Shader::LoadShaderBytecode(const std::wstring& fileName) {
		ID3DBlob* blob;
		DX::ThrowIfFailed(
			D3DReadFileToBlob(fileName.c_str(), &blob),
			fmt::format("Unable to load shader bytecode from {}", StringUtils::WStringToString(fileName))
		);

		return blob;
	}

	Shader::Shader(const std::wstring& fileName) {
		this->mBytecode = LoadShaderBytecode(fileName);
	}

	void PixelShader::Bind(const D3DCore& core) const {
		core.GetContext()->PSSetShader(this->mShader.Get(), nullptr, 0);
	}

	PixelShader::PixelShader(const D3DCore& core, const std::wstring& fileName) : Shader(fileName) {
		ID3D11PixelShader* pShader;
		spdlog::trace("Creating pixel shader from {}", StringUtils::WStringToString(fileName));
		auto hr = core.GetDevice()->CreatePixelShader(this->mBytecode->GetBufferPointer(), this->mBytecode->GetBufferSize(), nullptr, &pShader);
		if (FAILED(hr)) {
			spdlog::error("Failed to create pixel shader from source");
			throw std::runtime_error("Failed to create pixel shader.");
		}
		this->mShader = pShader;
	}

	void VertexShader::Bind(const D3DCore& core) const {
		core.GetContext()->VSSetShader(this->mShader.Get(), nullptr, 0);
	}

	VertexShader::VertexShader(const D3DCore& core, const std::wstring& fileName) : Shader(fileName) {
		ID3D11VertexShader* vShader;
		spdlog::trace("Creating vertex shader from {}", StringUtils::WStringToString(fileName));
		auto hr = core.GetDevice()->CreateVertexShader(this->mBytecode->GetBufferPointer(), this->mBytecode->GetBufferSize(), nullptr, &vShader);
		if (FAILED(hr)) {
			spdlog::error("Failed to create vertex shader from source");
			throw std::runtime_error("Failed to create vertex shader.");
		}
		this->mShader = vShader;
	}
}
