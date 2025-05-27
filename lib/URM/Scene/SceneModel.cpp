#include "pch.h"
#include "SceneModel.h"
#include <stdexcept>

#include "SceneMesh.h"
#include "Scene.h"

namespace URM::Scene {
	std::shared_ptr<URM::Core::ShaderProgram> SceneModel::DefaultShaderProgram = nullptr;
	std::shared_ptr<URM::Core::ModelLoaderLayout> SceneModel::DefaultInputLayout;

	void SceneModel::AddMeshRecursive(URM::Core::ModelLoaderNode& node, std::weak_ptr<SceneObject> parent) {
		auto newObject = std::make_shared<SceneObject>();
		newObject->GetTransform().SetWorldSpaceMatrix(node.transform);
		parent.lock()->AddChild(newObject);

		for (auto& mesh : node.meshes) {
			auto meshObject = std::make_shared<SceneMesh>(mesh, this->inputLayout, this->shader);
			newObject->AddChild(meshObject);
		}

		for (auto& child : node.children) {
			AddMeshRecursive(child, newObject);
		}
	}

	void SceneModel::OnAdded() {
		if (this->shader == nullptr) {
			this->shader = SceneModel::GetDefaultShader(this->GetScene().GetCore());
		}

		if(this->inputLayout == nullptr) {
			this->inputLayout = SceneModel::GetDefaultInputLayout(this->GetScene().GetCore());
		}

		auto& scene = GetScene();
		auto model = URM::Core::ModelLoader::LoadFromFile(scene.GetCore(), scene.GetAssetManager().texturePool, this->path);

		AddMeshRecursive(model, this->GetSelfPtr());
	}

	SceneModel::SceneModel(std::string path, std::shared_ptr<URM::Core::ShaderProgram> shader, std::shared_ptr<URM::Core::ModelLoaderLayout> layout) {
		this->path = path;
		this->shader = shader;
		this->inputLayout = layout;
	}

	// TODO: Async loading
	SceneModel::SceneModel(std::string path) {
		this->path = path;
	}
}