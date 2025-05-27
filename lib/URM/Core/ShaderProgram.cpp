#include "pch.h"
#include "ShaderProgram.h"

#include <fstream>
#include <d3dcompiler.h>

#pragma comment(lib, "d3dcompiler.lib")

namespace URM::Core {
	ComPtr<ID3DBlob> ShaderProgram::LoadShaderBytecode(const std::wstring& fileName) {
		ID3DBlob* blob;
		DX::ThrowIfFailed(
			D3DReadFileToBlob(fileName.c_str(), &blob),
			fmt::format("Unable to load shader bytecode from {}", StringUtils::WStringToString(fileName))
		);

		return blob;
	}

	void ShaderProgram::Bind(D3DCore& core) {
		core.GetContext()->VSSetShader(this->vertexShader.Get(), nullptr, 0);
		core.GetContext()->PSSetShader(this->pixelShader.Get(), nullptr, 0);
	}

	ShaderProgram::ShaderProgram(D3DCore& core, const std::wstring& vertexPath, const std::wstring& pixelPath)
	{
		ID3D11VertexShader* vShader;
		this->vsSource = LoadShaderBytecode(vertexPath);
		spdlog::trace("Creating vertex shader from {}", StringUtils::WStringToString(vertexPath));
		spdlog::trace("Vertex shader size: {} bytes", this->vsSource->GetBufferSize());
		auto hr = core.GetDevice()->CreateVertexShader(this->vsSource->GetBufferPointer(), this->vsSource->GetBufferSize(), nullptr, &vShader);
		if (FAILED(hr)) {
			spdlog::error("Failed to create vertex shader from source");
			throw std::runtime_error("Failed to create vertex shader.");
		}
		this->vertexShader = vShader;

		ID3D11PixelShader* pShader;
		this->psSource = LoadShaderBytecode(pixelPath);
		hr = core.GetDevice()->CreatePixelShader(this->psSource->GetBufferPointer(), this->psSource->GetBufferSize(), nullptr, &pShader);
		if (FAILED(hr)) {
			spdlog::error("Failed to create pixel shader from source");
			throw std::runtime_error("Failed to create pixel shader.");
		}
		this->pixelShader = pShader;
	}
}