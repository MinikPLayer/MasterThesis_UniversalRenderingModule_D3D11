#pragma once

#include <directxtk/SimpleMath.h>

namespace URM::Engine {
	using namespace DirectX::SimpleMath;

	struct alignas(16) PixelLight {
		alignas(16) Vector3 color;
		alignas(16) Vector3 position;
		alignas(4) float ambientIntensity;
		alignas(4) float diffuseIntensity;
		alignas(4) float specularIntensity;
		alignas(4) float attenuationExponent;
		alignas(4) float pbrIntensity;

		// ReSharper disable once CppPossiblyUninitializedMember
		PixelLight(Vector3 position = Vector3::Zero,
			Color color = Color(1, 1, 1),
			float ambient = 0.05f,
			float diffuse = 0.9f,
			float specular = 1.0f,
			float attenuationExponent = 2.0f,
			float pbrIntensity = 10.0f
		) : color(color.ToVector3()),
			position(position),
			ambientIntensity(ambient),
			diffuseIntensity(diffuse),
			specularIntensity(specular),
			attenuationExponent(attenuationExponent),
			pbrIntensity(pbrIntensity) {
		}
	};

	struct PixelLightBufferData {
		static constexpr UINT SEMANTIC_SHADER_CONSTANT_BUFFER_INDEX = 3;
		static constexpr int MAX_LIGHTS_COUNT = 64;

		alignas(4) uint32_t activeLightsCount = 0;
		alignas(16) PixelLight lights[MAX_LIGHTS_COUNT];
	};

	// Alignment rules: https://maraneshi.github.io/HLSL-ConstantBufferLayoutVisualizer/
	struct PixelConstantBufferData {
		static constexpr UINT SEMANTIC_SHADER_CONSTANT_BUFFER_INDEX = 1;

		alignas(4) Vector4 viewPosition;

		PixelConstantBufferData(Vector3 viewPos) : viewPosition(viewPos.x, viewPos.y, viewPos.z, 1.0f) {}
	};

	struct VertexConstantBufferData {
		static constexpr UINT SEMANTIC_SHADER_CONSTANT_BUFFER_INDEX = 0;

		// ReSharper disable once CppInconsistentNaming
		Matrix WVP;
		Matrix worldMatrix;
		Matrix inverseWorldMatrix;
	};
}