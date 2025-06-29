#include "pch.h"
#include "ModelObject.h"

#include "MeshObject.h"
#include "Scene.h"

namespace URM::Engine {
	std::shared_ptr<Core::ShaderProgram> ModelObject::mDefaultShaderProgram = nullptr;
	std::shared_ptr<Core::ModelLoaderLayout> ModelObject::mDefaultInputLayout;

	void ModelObject::AddMeshRecursive(const std::shared_ptr<Core::ModelLoaderNode>& node, const std::weak_ptr<SceneObject>& parent) {
		auto newObject = std::make_shared<SceneObject>();
		newObject->GetTransform().SetWorldSpaceMatrix(node->transform);
		parent.lock()->AddChild(newObject);

		for (auto& mesh : node->meshes) {
			auto meshObject = std::make_shared<MeshObject>(mesh, this->mInputLayout, this->mShader);
			newObject->AddChild(meshObject);
		}

		for (auto& child : node->children) {
			AddMeshRecursive(child, newObject);
		}
	}

	void ModelObject::OnAdded() {
		if (this->mShader == nullptr) {
			this->mShader = GetDefaultShader(this->GetScene().GetCore());
		}

		if (this->mInputLayout == nullptr) {
			this->mInputLayout = GetDefaultInputLayout(this->GetScene().GetCore());
		}

		auto& scene = GetScene();
		const auto model = Core::TimeUtils::TraceExecTimeMs<std::shared_ptr<Core::ModelLoaderNode>>(fmt::format("Load model: {}", this->mPath), [&]() {
			return scene.GetAssetManager().GetModel(this->mPath);
		});

		AddMeshRecursive(model, this->GetSelfPtr());
	}

	ModelObject::ModelObject(const std::string& path, const std::shared_ptr<Core::ShaderProgram>& shader, const std::shared_ptr<Core::ModelLoaderLayout>& layout) {
		this->mPath = path;
		this->mShader = shader;
		this->mInputLayout = layout;
	}

	// PLAN: Async loading
	ModelObject::ModelObject(const std::string& path) {
		this->mPath = path;
	}
}
