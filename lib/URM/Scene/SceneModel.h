#pragma once

#include "SceneObject.h"
#include <URM/Core/ModelLoader.h>
#include <string>

class SceneModel : public SceneObject {
	std::string path;

public:
	void OnAdded() override;

	SceneModel(std::string path);
};