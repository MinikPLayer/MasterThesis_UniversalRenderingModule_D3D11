#pragma once

#include <string>
#include "D3DCore.h"

#include <d3dcommon.h>
#include <variant>

namespace URM::Core {
	enum ShaderStages {
		VERTEX   = 0,
		PIXEL    = 1,
	};

	class Shader {
	protected:
		ComPtr<ID3DBlob> mBytecode;

		static ComPtr<ID3DBlob> LoadShaderBytecode(const std::wstring& fileName);
	public:
		virtual void Bind(const D3DCore& core) const = 0;

		ComPtr<ID3DBlob> GetBytecode() const {
			return this->mBytecode;
		}

		Shader(const std::wstring& fileName);
	};

	class PixelShader : public Shader {
		ComPtr<ID3D11PixelShader> mShader;

	public:
		void Bind(const D3DCore& core) const override;
		PixelShader(const D3DCore& core, const std::wstring& fileName);
	};

	class VertexShader : public Shader {
		ComPtr<ID3D11VertexShader> mShader;

	public:
		void Bind(const D3DCore& core) const override;
		VertexShader(const D3DCore& core, const std::wstring& fileName);
	};
}
