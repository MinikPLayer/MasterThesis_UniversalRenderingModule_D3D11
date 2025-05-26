#pragma once

#include "SceneObject.h"
#include <URM/Core/ModelLoader.h>

class SceneMesh : public SceneObject {
	Mesh<ModelLoaderVertexType> mesh;

public:
	void OnAdded() override;
	void OnDestroyed() override;

	Mesh<ModelLoaderVertexType> GetMesh() {
		return this->mesh;
	}

	SceneMesh(Mesh<ModelLoaderVertexType> mesh);
};