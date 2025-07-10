#pragma once

#include <directxtk/SimpleMath.h>
#include <variant>
#include <stdexcept>

#include "ShaderPipeline.h"
#include "D3DInputLayout.h"

#include "D3DConstantBuffer.h"

namespace URM::Core {


	struct MaterialPBRData {
		DirectX::SimpleMath::Color albedo;
		int useAlbedoTexture = 0;
		float metallic = 0.0f;
		float roughness = 0.5f;
		float ao = 1.0f;
	};

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

	class MaterialSimple : public Material {
	public:
		struct Data {
			alignas(4) int useAlbedoTexture = 0;
			alignas(4) int roughnessPowerExponent = 32;
			alignas(16) Color albedoColor = Color(1, 1, 1, 1);

			Data(int roughnessPowerExponent = 32, Color albedoColor = Color(1, 1, 1, 1)) 
				: roughnessPowerExponent(roughnessPowerExponent), albedoColor(albedoColor) {
			}
		};

	private:
		static std::shared_ptr<Core::PixelShader> defaultShader;

		std::shared_ptr<Core::PixelShader> GetShader(D3DCore& core) override;
	public:
		Data data;

		void UploadData(D3DCore& core, bool useAlbedoTexture) override;
		MaterialSimple(D3DCore& core, MaterialSimple::Data data = MaterialSimple::Data());
	};

	class MaterialPBR : public Material {
	public:
		struct Data {
			alignas(16) Color albedo = Color(1.0f, 1.0f, 1.0f, 1.0f);
			alignas(4) int useAlbedoTexture = 0;
			alignas(4) float metallic = 0.5f;
			alignas(4) float roughness = 0.5f;
			alignas(4) float ao = 1.0f;
		};

	private:
		static std::shared_ptr<Core::PixelShader> defaultShader;

		std::shared_ptr<Core::PixelShader> GetShader(D3DCore& core) override;

	public:
		Data data;

		void UploadData(D3DCore& core, bool useAlbedoTexture) override;
		MaterialPBR(D3DCore& core, MaterialPBR::Data data = MaterialPBR::Data());
	};
}