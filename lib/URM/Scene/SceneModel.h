#pragma once

#include "SceneObject.h"
#include <Core/ModelLoader.h>
#include <string>

class Model : public SceneObject {
	void AddMeshRecursive(ModelLoaderNode& node);

public:
	Model(Scene& scene, std::string path);
};