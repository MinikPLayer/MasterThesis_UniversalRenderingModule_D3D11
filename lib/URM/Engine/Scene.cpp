#include "pch.h"
#include "Scene.h"

namespace URM::Engine {
	std::vector<std::weak_ptr<SceneMesh>>& Scene::GetMeshes() {
		return this->meshes;
	}

	void Scene::PrintObjectsHierarchy() const {
		spdlog::info("\n\nObjects hierarchy:");
		rootObject->PrintHierarchy(0);
		spdlog::info("\n\n");
	}
}
