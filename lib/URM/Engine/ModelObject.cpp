#include "pch.h"
#include "ModelObject.h"

#include "MeshObject.h"
#include "Scene.h"

namespace URM::Engine {
	std::map<URM::Core::RenderingStage, std::shared_ptr<Core::ShaderPipeline>> ModelObject::mDefaultShaders = {};
	std::map<URM::Core::RenderingStage, std::shared_ptr<Core::ModelLoaderLayout>> ModelObject::mDefaultInputLayouts = {};

	void ModelObject::AddMeshRecursive(const std::shared_ptr<Core::ModelLoaderNode>& node, const std::weak_ptr<SceneObject>& parent) {
		auto newObject = std::make_shared<SceneObject>();
		newObject->GetTransform().SetWorldSpaceMatrix(node->transform);
		parent.lock()->AddChild(newObject);

		for (auto& mesh : node->meshes) {
			auto meshObject = std::make_shared<MeshObject>(mesh, this->mInputLayouts, this->mShaders);
			newObject->AddChild(meshObject);
		}

		for (auto& child : node->children) {
			AddMeshRecursive(child, newObject);
		}
	}

	std::shared_ptr<Core::ShaderPipeline> ModelObject::GetDefaultShader(Core::D3DCore& core, URM::Core::RenderingStage stage) {
		auto currentValue = mDefaultShaders[stage];
		if (currentValue == nullptr) {
			std::wstring pixelShaderPath;
			switch (stage)
			{
			case URM::Core::RenderingStage::GEOMETRY:
				pixelShaderPath = L"PixelShaderGeometryStage.cso";
				break;
			case URM::Core::RenderingStage::LIGHTING:
				pixelShaderPath = L"PixelShaderLightingStage.cso";
				break;
			default:
				break;
			}

			currentValue = mDefaultShaders[stage] = std::make_shared<Core::ShaderPipeline>(core, L"SimpleVertexShader.cso", pixelShaderPath);
		}

		return currentValue;
	}


	std::shared_ptr<Core::ModelLoaderLayout> ModelObject::GetDefaultInputLayout(Core::D3DCore& core, URM::Core::RenderingStage stage) {
		auto currentValue = mDefaultInputLayouts[stage];
		if (currentValue == nullptr) {
			auto defaultShader = GetDefaultShader(core, stage);
			currentValue = mDefaultInputLayouts[stage] = std::make_shared<Core::ModelLoaderLayout>(core, *(defaultShader.get()));
		}

		return currentValue;
	}

	void ModelObject::OnAdded() {
		if (this->mShaders[URM::Core::RenderingStage::GEOMETRY] == nullptr) {
			this->mShaders[URM::Core::RenderingStage::GEOMETRY] = GetDefaultShader(this->GetScene().GetCore(), URM::Core::RenderingStage::GEOMETRY);
		}

		if (this->mShaders[URM::Core::RenderingStage::LIGHTING] == nullptr) {
			this->mShaders[URM::Core::RenderingStage::LIGHTING] = GetDefaultShader(this->GetScene().GetCore(), URM::Core::RenderingStage::LIGHTING);
		}

		if (this->mInputLayouts[URM::Core::RenderingStage::GEOMETRY] == nullptr) {
			this->mInputLayouts[URM::Core::RenderingStage::GEOMETRY] = GetDefaultInputLayout(this->GetScene().GetCore(), URM::Core::RenderingStage::GEOMETRY);
		}

		if (this->mInputLayouts[URM::Core::RenderingStage::LIGHTING] == nullptr) {
			this->mInputLayouts[URM::Core::RenderingStage::LIGHTING] = GetDefaultInputLayout(this->GetScene().GetCore(), URM::Core::RenderingStage::LIGHTING);
		}

		auto& scene = GetScene();
		const auto model = Core::TimeUtils::TraceExecTimeMs<std::shared_ptr<Core::ModelLoaderNode>>(fmt::format("Load model: {}", this->mPath), [&]() {
			return scene.GetAssetManager().GetModel(this->mPath);
		});

		AddMeshRecursive(model, this->GetSelfPtr());
	}

	ModelObject::ModelObject(const std::string& path, const std::map<URM::Core::RenderingStage, std::shared_ptr<Core::ShaderPipeline>> shaders, const std::map<URM::Core::RenderingStage, std::shared_ptr<Core::ModelLoaderLayout>> inputLayouts) {
		this->mPath = path;
		this->mShaders = shaders;
		this->mInputLayouts = inputLayouts;
	}

	// PLAN: Async loading
	ModelObject::ModelObject(const std::string& path) {
		this->mPath = path;
	}
}
