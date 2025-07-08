#include "pch.h"
#include "ModelObject.h"

#include "MeshObject.h"
#include "Scene.h"

namespace URM::Engine {
	std::shared_ptr<Core::VertexShader> ModelObject::mDefaultVertexShader = nullptr;
	std::shared_ptr<Core::ModelLoaderLayout> ModelObject::mDefaultInputLayout = nullptr;
	std::shared_ptr<Core::Material> ModelObject::mDefaultMaterial = nullptr;

	std::unordered_map<std::string, ModelObject::MaterialData> ModelObject::materialPropertiesMap = {
		{ "clr.diffuse", MaterialData(MaterialProperties::DIFFUSE_COLOR, 4) },
		{ "mat.metallicFactor", MaterialData(MaterialProperties::METALLIC_FACTOR, 1) },
		{ "mat.roughnessFactor", MaterialData(MaterialProperties::ROUGHNESS_FACTOR, 1) },
	};

	std::shared_ptr<Core::Material> ModelObject::TryDeduceMaterial(URM::Core::D3DCore& core, std::vector<URM::Core::MaterialProperty> properties) {
		if(properties.size() == 0) {
			return nullptr;
		}

		auto pbrMat = std::shared_ptr<Core::MaterialPBR>(new Core::MaterialPBR(core));
		bool anythingChanged = false;
		for (auto& prop : properties) {
			// Common name format:
			// - $mat.metallicFactor
			// - $clr.diffuse
			
			if (prop.name.size() == 0 || prop.name[0] != '$') {
				continue;
			}

			// Skip first character
			auto name = prop.name.substr(1);
			if (name.starts_with("mat.") || name.starts_with("clr.")) {
				spdlog::trace("Property material: {}", name);

				if (!materialPropertiesMap.count(name)) {
					continue;
				}
				auto value = prop.GetFloatArray();
				auto prop = materialPropertiesMap.at(name);
				if (value.length != prop.expectedArrayLength) {
					spdlog::error("Invalid data length for {}: {}", name, value.length);
					continue;
				}

				switch (prop.property) {
				case MaterialProperties::DIFFUSE_COLOR:
					pbrMat->data.albedo = Color(value.data[0], value.data[1], value.data[2], value.data[3]);
					break;

				case MaterialProperties::METALLIC_FACTOR:
					pbrMat->data.metallic = value.data[0];
					break;

				case MaterialProperties::ROUGHNESS_FACTOR:
					pbrMat->data.metallic = value.data[0];
					break;

				default:
					spdlog::warn("Invalid material property: {}", (int)prop.property);
					continue;
				}

				anythingChanged = true;
			}
		}

		if (anythingChanged) {
			spdlog::trace("Successfully deduced some material information for model {}.", this->mPath);
			return pbrMat;
		}

		spdlog::trace("Deducing material properties failed for model {}.", this->mPath);
		return nullptr;
	}

	void ModelObject::AddMeshRecursive(const std::shared_ptr<Core::ModelLoaderNode>& node, const std::weak_ptr<SceneObject>& parent) {
		auto newObject = std::make_shared<SceneObject>();
		newObject->GetTransform().SetWorldSpaceMatrix(node->transform);
		parent.lock()->AddChild(newObject);

		auto& core = this->GetScene().GetCore();
		for (auto& mesh : node->meshes) {
			std::shared_ptr<Core::Material> material = nullptr;
			if (this->mTryDeduceMaterial) {
				material = TryDeduceMaterial(core, mesh->materialProperties);
			}

			if (material == nullptr) {
				material = this->mMaterial;
			}

			auto meshObject = std::shared_ptr<MeshObject>(new MeshObject(mesh, this->mVertexShader, this->mInputLayout, material));
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
	ModelObject::ModelObject(const std::string& path, bool tryDeduceMaterial) {
		this->mPath = path;

		this->mTryDeduceMaterial = tryDeduceMaterial;
	}
}
