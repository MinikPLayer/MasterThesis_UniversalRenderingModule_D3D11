#pragma once

#include "SceneObject.h"
#include <vector>
#include <memory>

class Scene {
	std::shared_ptr<SceneObject> rootObject;

public:
	std::weak_ptr<SceneObject> GetRoot() const {
		return rootObject;
	}
};