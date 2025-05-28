#pragma once

#include <URM/Core/Window.h>
#include <URM/Core/D3DCore.h>
#include <URM/Core/D3DViewport.h>
#include <URM/Core/D3DRasterizerState.h>
#include <URM/Core/D3DConstantBuffer.h>

#include <functional>
#include <chrono>
#include <optional>

#include "Timer.h"
#include "Scene.h"

namespace URM::Engine {
	struct RenderingParams {
		Color clearColor;
		URM::Core::PrimitiveTopologies toplogy = URM::Core::PrimitiveTopologies::TRIANGLE_LIST;
		URM::Core::D3DRasterizerState rasterizerState;
		URM::Core::D3DViewport viewport;
		URM::Core::D3DSampler albedoTextureSampler;

		RenderingParams() {}
	};

	class Engine : NonCopyable {
		Timer timer;

		URM::Core::D3DCore core;
		URM::Engine::Scene scene;

		URM::Core::D3DConstantBuffer vertexConstantBuffer;
		URM::Core::D3DConstantBuffer pixelConstantBuffer;

	public:
		int VSyncInterval = 0;
		RenderingParams RenderParameters;

		std::function<void(Engine& engine)> OnUpdate;

		Timer& GetTimer() {
			return timer;
		}

		URM::Core::D3DCore& GetCore() {
			return core;
		}

		URM::Engine::Scene& GetScene() {
			return scene;
		}

		void Update();

		void Clear(Color color);
		void Clear();
		void Draw(RenderingParams& params, std::vector<std::weak_ptr<URM::Engine::SceneMesh>>& meshes);
		void Draw(RenderingParams& params);
		void Present(int verticalSyncInterval);

		bool ShouldClose();

		Engine(URM::Core::WindowCreationParams windowParams);

		void RunLoop();
	};
}