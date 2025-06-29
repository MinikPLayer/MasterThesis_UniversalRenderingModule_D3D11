#include "pch.h"
#include "Scene.h"

namespace URM::Engine {
	std::vector<std::weak_ptr<MeshObject>>& Scene::GetMeshes() {
		return this->mMeshes;
	}

	void Scene::PrintObjectsHierarchy() const {
		spdlog::info("\n\nObjects hierarchy:");
		mRootObject->PrintHierarchy(0);
		spdlog::info("\n\n");
	}
	
	std::weak_ptr<CameraObject> Scene::GetMainCamera() {
		return this->mMainCamera;
	}
	
	void Scene::SetMainCamera(const std::weak_ptr<CameraObject>& camera) {
		this->mMainCamera = camera;
	}

	std::vector<std::weak_ptr<LightObject>>& Scene::GetLights() {
		return this->mLights;
	}
}
