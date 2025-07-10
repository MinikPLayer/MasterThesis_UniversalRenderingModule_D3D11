#pragma once

#include <directxtk/SimpleMath.h>
#include <variant>
#include <stdexcept>

#include "ShaderPipeline.h"
#include "D3DInputLayout.h"

#include "D3DConstantBuffer.h"

namespace URM::Core {
	class Material {
	protected:
		std::shared_ptr<Core::PixelShader> mShader = nullptr;

		D3DConstantBuffer mConstantBuffer;

		virtual std::shared_ptr<Core::PixelShader> GetShader(D3DCore& core) = 0;
	public:
		static constexpr UINT SEMANTIC_SHADER_CONSTANT_BUFFER_INDEX = 2;

		void Bind(D3DCore& core, UINT bufferIndex);
		virtual void UploadData(D3DCore& core, bool useAlbedoTexture) = 0;

		Material(D3DConstantBuffer constantBuffer) : mConstantBuffer(std::move(constantBuffer)) {}
	};
}