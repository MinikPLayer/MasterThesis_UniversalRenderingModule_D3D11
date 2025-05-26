#pragma once

#include "SceneObject.h"
#include "AssetManager.h"
#include <vector>
#include <memory>
#include <URM/Core/D3DCore.h>

class SceneMesh;
class SceneObject;
class Scene {
	friend class SceneMesh;

	std::shared_ptr<SceneObject> rootObject;
	std::vector<std::weak_ptr<SceneMesh>> meshes;

	AssetManager assetManager;
	D3DCore& core;

public:
	// Disable copy constructor and assignment operator
	Scene(const Scene&) = delete;
	Scene& operator=(const Scene&) = delete;

	std::vector<std::weak_ptr<SceneMesh>>& GetMeshes();

	void PrintObjectsHierarchy();

	AssetManager& GetAssetManager() {
		return this->assetManager;
	}

	D3DCore& GetCore() {
		return this->core;
	}

	std::weak_ptr<SceneObject> GetRoot() const {
		return this->rootObject;
	}

	Scene(D3DCore& core);
};