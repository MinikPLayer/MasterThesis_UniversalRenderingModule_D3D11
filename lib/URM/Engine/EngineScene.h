#pragma once

#include <URM/Scene/Scene.h>
#include <URM/Core/Utils.h>
#include "CameraObject.h"

namespace URM::Engine {
	struct EngineSceneData {
	private:
		friend class CameraObject;

		std::weak_ptr<URM::Engine::CameraObject> mainCamera;

	public:
		std::weak_ptr<URM::Engine::CameraObject> GetMainCamera();
	};

	class EngineScene : public URM::Scene::Scene {
		EngineSceneData data;

		URM::Scene::Scene::CustomData GetCustomData_Internal() override {
			return URM::Scene::Scene::CustomData(
				&data,
				URM::Core::TypeUtils::GetTypeCode<EngineSceneData>()
			);
		}
	};
}