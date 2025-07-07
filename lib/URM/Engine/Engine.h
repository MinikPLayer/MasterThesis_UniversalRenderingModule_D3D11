#pragma once

#include <URM/Core/Window.h>
#include <URM/Core/D3DCore.h>
#include <URM/Core/D3DViewport.h>
#include <URM/Core/D3DRasterizerState.h>
#include <URM/Core/D3DConstantBuffer.h>
#include <URM/Core/D3DBlendState.h>
#include <URM/Core/D3DDepthStencilState.h>
#include <URM/Core/Material.h>

#include <functional>

#include "Timer.h"
#include "Scene.h"

namespace URM::Engine {
	struct alignas(16) PixelLightPBR {
		alignas(16) Vector3 color;
		alignas(16) Vector3 position;

		PixelLightPBR(Vector3 position = Vector3::Zero, Color color = Color(1, 1, 1)) 
			: color(color.ToVector3()), position(position) {}
	};

	struct alignas(16) PixelLight {
		alignas(16) Vector3 color;
		alignas(16) Vector3 position;
		alignas(4) float ambientIntensity;
		alignas(4) float diffuseIntensity;
		alignas(4) float specularIntensity;

		// ReSharper disable once CppInconsistentNaming
		int _padding_;

		// ReSharper disable once CppPossiblyUninitializedMember
		PixelLight(Vector3 position = Vector3::Zero,
			Color color = Color(1, 1, 1),
			float ambient = 0.05f,
			float diffuse = 0.9f,
			float specular = 1.0f
		) : color(color.ToVector3()), position(position), ambientIntensity(ambient), diffuseIntensity(diffuse), specularIntensity(specular) {
		}
	};

	struct PixelLightBufferData {
		static constexpr int MAX_LIGHTS_COUNT = 64;

		alignas(4) uint32_t activeLightsCount = 0;
		alignas(16) PixelLight lights[MAX_LIGHTS_COUNT];
	};

	struct PixelLightBufferDataPBR {
		static constexpr int MAX_LIGHTS_COUNT = 64;

		alignas(4) uint32_t activeLightsCount = 0;
		alignas(16) PixelLightPBR lights[MAX_LIGHTS_COUNT];
	};

	struct PixelMaterialBufferDataPBR {
		alignas(16) Color albedo;
		alignas(4) int useAlbedoTexture;
		alignas(4) float metallic;
		alignas(4) float roughness;
		alignas(4) float ao;

		PixelMaterialBufferDataPBR(Color albedo = Color(1, 1, 1, 1), int useAlbedoTexture = 0, float metallic = 0.0f, float roughness = 0.5f, float ao = 1.0f) 
			: albedo(albedo), useAlbedoTexture(useAlbedoTexture), metallic(metallic), roughness(roughness), ao(ao) {}
	};

	struct PixelMaterialBufferData {
		alignas(4) int useAlbedoTexture = 0;
		alignas(4) int roughnessPowerExponent;
		alignas(16) Color albedoColor = Color(1, 1, 1, 1);

		PixelMaterialBufferData(Color albedoColor = Color(1, 1, 1, 1), int roughnessPowerExponent = 32) 
			: albedoColor(albedoColor), roughnessPowerExponent(roughnessPowerExponent) {}
	};

	// Alignment rules: https://maraneshi.github.io/HLSL-ConstantBufferLayoutVisualizer/
	struct PixelConstantBufferData {
		alignas(4) Vector4 viewPosition;

		PixelConstantBufferData(Vector3 viewPos) : viewPosition(viewPos.x, viewPos.y, viewPos.z, 1.0f) {}
	};
	
	struct RenderingParamsPerPass {
		Core::D3DBlendState blendState;
		Core::D3DDepthStencilState depthStencilState;
	};

	struct RenderingParams {
		Color clearColor;
		Core::PrimitiveTopologies topology = Core::PrimitiveTopologies::TRIANGLE_LIST;
		Core::D3DRasterizerState rasterizerState;
		Core::D3DViewport viewport;
		Core::D3DSampler albedoTextureSampler;
		RenderingParamsPerPass geometryPassParams;
		RenderingParamsPerPass lightingPassParams;

		RenderingParams() = default;
	};

	class Engine : NonCopyable {
		Timer mTimer;

		Core::D3DCore mCore;
		Scene mScene;

		Core::D3DConstantBuffer mVertexConstantBuffer;
		Core::D3DConstantBuffer mPixelConstantBuffer;
		Core::D3DConstantBuffer mPixelMaterialConstantBuffer;
		Core::D3DConstantBuffer mPixelLightsConstantBuffer;
		
		bool mNoLightsWarningShown = false;
	public:
		int vSyncInterval = 0;
		RenderingParams renderParameters;

		std::function<void(Engine& engine)> onUpdate;

		Timer& GetTimer() {
			return mTimer;
		}

		Core::D3DCore& GetCore() {
			return mCore;
		}

		Scene& GetScene() {
			return mScene;
		}

		void Update();

		void Clear(Color color);
		void Clear();
		void Draw(RenderingParams& params, std::weak_ptr<CameraObject> mainCamera, std::vector<std::weak_ptr<MeshObject>>& meshes, std::vector<std::weak_ptr<LightObject>>& lights);
		void Draw(RenderingParams& params, Scene& scene);
		void Draw(RenderingParams& params);
		void Present(int verticalSyncInterval);

		bool ShouldClose() const;

		explicit Engine(const Core::WindowCreationParams& windowParams);

		void RunLoop();
		void RunLoopTrace();
	};
}
