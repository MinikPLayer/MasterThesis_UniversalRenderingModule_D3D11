#pragma once

#include "Material.h"

namespace URM::Core {

	struct MaterialSimpleData {
		alignas(4) int useAlbedoTexture = 0;
		alignas(4) int roughnessPowerExponent = 32;
		alignas(16) Color albedoColor = Color(1, 1, 1, 1);

		MaterialSimpleData(int roughnessPowerExponent = 32, Color albedoColor = Color(1, 1, 1, 1))
			: roughnessPowerExponent(roughnessPowerExponent), albedoColor(albedoColor) {
		}
	};

	class MaterialSimple : public MaterialWithData<MaterialSimpleData> {
		const wchar_t* GetShaderFilePath() const override;
	public:
		void UploadData(D3DCore& core, bool useAlbedoTexture) override;
		MaterialSimple(D3DCore& core, MaterialSimpleData data = MaterialSimpleData());
	};

	struct MaterialPBRData {
		alignas(16) Color albedo = Color(1.0f, 1.0f, 1.0f, 1.0f);
		alignas(4) int useAlbedoTexture = 0;
		alignas(4) float metallic = 0.5f;
		alignas(4) float roughness = 0.5f;
		alignas(4) float ao = 1.0f;
	};

	class MaterialPBR : public MaterialWithData<MaterialPBRData> {
		const wchar_t* GetShaderFilePath() const override;
	public:
		void UploadData(D3DCore& core, bool useAlbedoTexture) override;
		MaterialPBR(D3DCore& core, MaterialPBRData data = MaterialPBRData());
	};
}