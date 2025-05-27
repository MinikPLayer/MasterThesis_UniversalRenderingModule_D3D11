#include "pch.h"
#include "SceneModel.h"
#include <stdexcept>

#include "SceneMesh.h"
#include "Scene.h"

namespace URM::Scene {
	void AddMeshRecursive(URM::Core::ModelLoaderNode& node, std::weak_ptr<SceneObject> parent) {
		auto newObject = std::make_shared<SceneObject>();
		newObject->GetTransform().SetWorldSpaceMatrix(node.transform);
		parent.lock()->AddChild(newObject);

		for (auto& mesh : node.meshes) {
			auto meshObject = std::make_shared<SceneMesh>(mesh);
			newObject->AddChild(meshObject);
		}

		for (auto& child : node.children) {
			AddMeshRecursive(child, newObject);
		}
	}

	void SceneModel::OnAdded() {
		auto& scene = GetScene();
		auto model = URM::Core::ModelLoader::LoadFromFile(scene.GetCore(), scene.GetAssetManager().texturePool, this->path);

		AddMeshRecursive(model, this->GetSelfPtr());
	}

	// TODO: Async loading
	SceneModel::SceneModel(std::string path) {
		this->path = path;
	}
}