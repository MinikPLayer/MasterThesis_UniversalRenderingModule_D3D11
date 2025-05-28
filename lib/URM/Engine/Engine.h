#pragma once

#include <URM/Core/Window.h>
#include <URM/Core/D3DCore.h>
#include <URM/Scene/Scene.h>
#include <URM/Core/D3DViewport.h>
#include <URM/Core/Color.h>
#include <URM/Core/D3DRasterizerState.h>
#include <URM/Core/D3DConstantBuffer.h>

#include <functional>
#include <chrono>
#include <optional>

#include "Timer.h"

namespace URM::Engine {
	struct RenderingParams {
		URM::Core::Color clearColor;
		URM::Core::PrimitiveTopologies toplogy = URM::Core::PrimitiveTopologies::TRIANGLE_LIST;
		URM::Core::D3DRasterizerState rasterizerState;
		URM::Core::D3DViewport viewport;
		URM::Core::D3DSampler albedoTextureSampler;

		RenderingParams() {}
	};

	class Engine {
		Timer timer;

		URM::Core::D3DCore core;
		URM::Scene::Scene scene;

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

		URM::Scene::Scene& GetScene() {
			return scene;
		}

		void Update();

		void Clear(URM::Core::Color color);
		void Clear();
		void Draw(RenderingParams params, std::vector<std::weak_ptr<URM::Scene::SceneMesh>> meshes);
		void Draw(RenderingParams params);
		void Present(int verticalSyncInterval);

		bool ShouldClose();

		// Disable copy constructor and assignment operator
		Engine(const Engine&) = delete;
		Engine& operator=(const Engine&) = delete;

		Engine(URM::Core::WindowCreationParams windowParams);

		void RunLoop();
	};
}