#include "pch.h"
#include "Scene.h"

void Scene::PrintObjectsHierarchy() {
	spdlog::info("\n\nObjects hierarchy:");
	rootObject->__PrintHierarchy__(0);
	spdlog::info("\n\n");
}
