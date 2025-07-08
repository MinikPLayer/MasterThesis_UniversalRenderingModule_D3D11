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
			pbrIntensity(pbrIntensity) {}
	};

	struct PixelLightBufferData {
		static constexpr int MAX_LIGHTS_COUNT = 64;

		alignas(4) uint32_t activeLightsCount = 0;
		alignas(16) PixelLight lights[MAX_LIGHTS_COUNT];
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
