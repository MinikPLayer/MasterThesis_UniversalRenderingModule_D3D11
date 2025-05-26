#pragma once

#include "SceneObject.h"
#include "AssetManager.h"
#include <vector>
#include <memory>
#include <Core/D3DCore.h>

class Scene {
	std::shared_ptr<SceneObject> rootObject;

	AssetManager assetManager;
	D3DCore& core;

public:
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

	Scene(D3DCore& core) : core(core) {}
};