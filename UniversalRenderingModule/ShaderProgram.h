#pragma once

#include <vector>
#include <string>
#include <D3DCore.h>

#include <d3dcommon.h>

class ShaderProgram {
	ComPtr<ID3D11VertexShader> vertexShader;
	ComPtr<ID3D11PixelShader> pixelShader;

	ComPtr<ID3DBlob> vsSource;
	ComPtr<ID3DBlob> psSource;

	ComPtr<ID3DBlob> LoadShaderBytecode(const std::wstring& fileName);

public:
	// TODO: Add function to bind to the context

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