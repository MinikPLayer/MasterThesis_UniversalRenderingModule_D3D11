#include "pch.h"
#include "ShaderPipeline.h"

#include <d3dcompiler.h>

#pragma comment(lib, "d3dcompiler.lib")

namespace URM::Core {
	ComPtr<ID3DBlob> ShaderPipeline::LoadShaderBytecode(const std::wstring& fileName) {
		ID3DBlob* blob;
		DX::ThrowIfFailed(
			D3DReadFileToBlob(fileName.c_str(), &blob),
			fmt::format("Unable to load shader bytecode from {}", StringUtils::WStringToString(fileName))
		);

		return blob;
	}

	void ShaderPipeline::Bind(const D3DCore& core) const {
		core.GetContext()->VSSetShader(this->mVertexShader.Get(), nullptr, 0);
		core.GetContext()->PSSetShader(this->mPixelShader.Get(), nullptr, 0);
	}

	ShaderPipeline::ShaderPipeline(const D3DCore& core, const std::wstring& vertexPath, const std::wstring& pixelPath) {
		ID3D11VertexShader* vShader;
		this->mVertexSource = LoadShaderBytecode(vertexPath);
		spdlog::trace("Creating vertex shader from {}", StringUtils::WStringToString(vertexPath));
		spdlog::trace("Vertex shader size: {} bytes", this->mVertexSource->GetBufferSize());
		auto hr = core.GetDevice()->CreateVertexShader(this->mVertexSource->GetBufferPointer(), this->mVertexSource->GetBufferSize(), nullptr, &vShader);
		if (FAILED(hr)) {
			spdlog::error("Failed to create vertex shader from source");
			throw std::runtime_error("Failed to create vertex shader.");
		}
		this->mVertexShader = vShader;

		ID3D11PixelShader* pShader;
		this->mPixelSource = LoadShaderBytecode(pixelPath);
		hr = core.GetDevice()->CreatePixelShader(this->mPixelSource->GetBufferPointer(), this->mPixelSource->GetBufferSize(), nullptr, &pShader);
		if (FAILED(hr)) {
			spdlog::error("Failed to create pixel shader from source");
			throw std::runtime_error("Failed to create pixel shader.");
		}
		this->mPixelShader = pShader;
	}
}
