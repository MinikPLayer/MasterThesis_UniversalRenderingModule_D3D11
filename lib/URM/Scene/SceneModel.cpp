#include "pch.h"
#include "SceneModel.h"
#include <stdexcept>

#include "SceneMesh.h"
#include "Scene.h"

void Model::AddMeshRecursive(ModelLoaderNode& node) {
	for (auto& mesh : node.meshes) {
		auto meshObject = std::make_shared<SceneMesh>(mesh);
		this->AddChild(meshObject);
	}
}

// TODO: Async loading
Model::Model(Scene& scene, std::string path) {
	auto model = ModelLoader::LoadFromFile(scene.GetCore(), scene.GetAssetManager().texturePool, path);
	this->AddMeshRecursive(model);
}
