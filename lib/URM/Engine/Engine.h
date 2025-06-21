#pragma once

#include <URM/Core/Window.h>
#include <URM/Core/D3DCore.h>
#include <URM/Core/D3DViewport.h>
#include <URM/Core/D3DRasterizerState.h>
#include <URM/Core/D3DConstantBuffer.h>

#include <functional>

#include "Timer.h"
#include "Scene.h"

namespace URM::Engine {
	struct RenderingParams {
		Color clearColor;
		Core::PrimitiveTopologies topology = Core::PrimitiveTopologies::TRIANGLE_LIST;
		Core::D3DRasterizerState rasterizerState;
		Core::D3DViewport viewport;
		Core::D3DSampler albedoTextureSampler;

		RenderingParams() = default;
	};

	class Engine : NonCopyable {
		Timer mTimer;

		Core::D3DCore mCore;
		Scene mScene;

		Core::D3DConstantBuffer mVertexConstantBuffer;
		Core::D3DConstantBuffer mPixelConstantBuffer;
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
		void Draw(RenderingParams& params, std::weak_ptr<CameraObject> mainCamera, std::vector<std::weak_ptr<SceneMesh>>& meshes, std::vector<std::weak_ptr<Light>>& lights);
		void Draw(RenderingParams& params, Scene& scene);
		void Draw(RenderingParams& params);
		void Present(int verticalSyncInterval);

		bool ShouldClose();

		explicit Engine(const Core::WindowCreationParams& windowParams);

		void RunLoop();
	};
}
