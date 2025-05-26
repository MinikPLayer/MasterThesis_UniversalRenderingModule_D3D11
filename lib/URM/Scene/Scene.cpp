#include "pch.h"
#include "Scene.h"

std::vector<std::weak_ptr<SceneMesh>>& Scene::GetMeshes() {
	return this->meshes;
}

void Scene::PrintObjectsHierarchy() {
	spdlog::info("\n\nObjects hierarchy:");
	rootObject->__PrintHierarchy__(0);
	spdlog::info("\n\n");
}

Scene::Scene(D3DCore& core) : core(core) {
	this->rootObject = std::make_shared<SceneObject>();
	this->rootObject->scene = *this;
	this->rootObject->self = this->rootObject;
}