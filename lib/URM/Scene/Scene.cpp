#include "pch.h"
#include "Scene.h"

namespace URM::Scene {
	std::vector<std::weak_ptr<SceneMesh>>& Scene::GetMeshes() {
		return this->meshes;
	}

	void Scene::PrintObjectsHierarchy() {
		spdlog::info("\n\nObjects hierarchy:");
		rootObject->__PrintHierarchy__(0);
		spdlog::info("\n\n");
	}
}