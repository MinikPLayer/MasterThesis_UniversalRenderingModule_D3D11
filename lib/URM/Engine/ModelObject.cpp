#include "pch.h"
#include "ModelObject.h"

#include "MeshObject.h"
#include "Scene.h"

namespace URM::Engine {
	std::shared_ptr<Core::VertexShader> ModelObject::mDefaultVertexShader = nullptr;
	std::shared_ptr<Core::ModelLoaderLayout> ModelObject::mDefaultInputLayout = nullptr;
	std::shared_ptr<Core::Material> ModelObject::mDefaultMaterial = nullptr;

	void ModelObject::AddMeshRecursive(const std::shared_ptr<Core::ModelLoaderNode>& node, const std::weak_ptr<SceneObject>& parent) {
		auto newObject = std::make_shared<SceneObject>();
		newObject->GetTransform().SetWorldSpaceMatrix(node->transform);
		parent.lock()->AddChild(newObject);

		for (auto& mesh : node->meshes) {
			auto meshObject = std::shared_ptr<MeshObject>(new MeshObject(mesh, this->mVertexShader, this->mInputLayout, this->mDefaultMaterial));
			newObject->AddChild(meshObject);
		}

		for (auto& child : node->children) {
			AddMeshRecursive(child, newObject);
		}
	}

	std::shared_ptr<Core::Material> ModelObject::GetDefaultMaterial(Core::D3DCore& core) {
		if (!mDefaultMaterial) {
			mDefaultMaterial = std::shared_ptr<Core::Material>(new Core::MaterialSimple(core));
		}

		return mDefaultMaterial;
	}

	std::shared_ptr<Core::VertexShader> ModelObject::GetDefaultVertexShader(Core::D3DCore& core) {
		if (!mDefaultVertexShader) {
			mDefaultVertexShader = std::shared_ptr<Core::VertexShader>(new Core::VertexShader(core, L"SimpleVertexShader.cso"));
		}

		return mDefaultVertexShader;
	}

	std::shared_ptr<Core::ModelLoaderLayout> ModelObject::GetDefaultInputLayout(Core::D3DCore& core) {
		if (!mDefaultInputLayout) {
			auto defaultShader = GetDefaultVertexShader(core);
			mDefaultInputLayout = std::shared_ptr<Core::ModelLoaderLayout>(new Core::ModelLoaderLayout(core, *(defaultShader.get())));
		}

		return mDefaultInputLayout;
	}

	void ModelObject::OnAdded() {
		if (this->mMaterial == nullptr) {
			this->mMaterial = GetDefaultMaterial(this->GetScene().GetCore());
		}

		if (this->mInputLayout == nullptr) {
			this->mInputLayout = GetDefaultInputLayout(this->GetScene().GetCore());
		}

		if(this->mVertexShader == nullptr) {
			this->mVertexShader = GetDefaultVertexShader(this->GetScene().GetCore());
		}

		auto& scene = GetScene();
		const auto model = Core::TimeUtils::TraceExecTimeMs<std::shared_ptr<Core::ModelLoaderNode>>(fmt::format("Load model: {}", this->mPath), [&]() {
			return scene.GetAssetManager().GetModel(this->mPath);
		});

		AddMeshRecursive(model, this->GetSelfPtr());
	}

	ModelObject::ModelObject(const std::string& path, const std::shared_ptr<Core::Material>& material, const std::shared_ptr<Core::VertexShader>& vertexShader, const std::shared_ptr<Core::ModelLoaderLayout>& layout) {
		this->mPath = path;
		this->mMaterial = material;
		this->mInputLayout = layout;
		this->mVertexShader = vertexShader;
	}

	// PLAN: Async loading
	ModelObject::ModelObject(const std::string& path) {
		this->mPath = path;
	}
}
