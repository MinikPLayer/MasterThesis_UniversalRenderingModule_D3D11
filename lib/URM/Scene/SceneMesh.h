#pragma once

#include "SceneObject.h"
#include <Core/ModelLoader.h>

class SceneMesh : public SceneObject {
	Mesh<ModelLoaderVertexType> mesh;

public:

	SceneMesh(Scene& scene, Mesh<ModelLoaderVertexType> mesh, std::string name);
};