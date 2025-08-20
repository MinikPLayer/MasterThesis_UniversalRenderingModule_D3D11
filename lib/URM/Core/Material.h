#pragma once

#include <directxtk/SimpleMath.h>
#include <variant>
#include <stdexcept>

#include "ShaderPipeline.h"
#include "D3DInputLayout.h"

#include "D3DConstantBuffer.h"

namespace URM::Core {
	class Material {
		static std::unordered_map<std::wstring, std::shared_ptr<PixelShader>> mPixelShadersCache;

		std::shared_ptr<Core::PixelShader> mShader = nullptr;
		std::shared_ptr<Core::PixelShader> GetShader(D3DCore& core);

	protected:
		virtual const wchar_t* GetShaderFilePath() const = 0;
		virtual void BindData(D3DCore& core, UINT bufferIndex) {}
	public:
		static constexpr UINT SEMANTIC_SHADER_CONSTANT_BUFFER_INDEX = 2;

		static void ClearShadersCache() {
			mPixelShadersCache.clear();
		}

		void Bind(D3DCore& core, UINT bufferIndex);
		virtual void Prepare(D3DCore& core, bool useAlbedoTexture = false) {}

		Material() = default;
	};

	template<typename T>
	class MaterialWithData : public Material {
	protected:
		D3DConstantBuffer mConstantBuffer;
		virtual void UploadData(D3DCore& core, bool useAlbedoTexture) {}

		void Prepare(D3DCore& core, bool useAlbedoTexture = false) override {
			this->UploadData(core, useAlbedoTexture);
		}

		void BindData(D3DCore& core, UINT bufferIndex) override {
			this->mConstantBuffer.Bind(core, bufferIndex);
		}

	public:
		T data;

		MaterialWithData(D3DCore& core, T data) : mConstantBuffer(D3DConstantBuffer::Create<T>(core, PIXEL)), data(data) {}
		MaterialWithData(D3DCore& core) : mConstantBuffer(D3DConstantBuffer::Create<T>(core, PIXEL)) {}
	};
}