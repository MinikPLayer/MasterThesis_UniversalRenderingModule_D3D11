#pragma once

#include "SceneObject.h"
#include <URM/Core/ModelLoader.h>
#include <URM/Core/D3DInputLayout.h>
#include <URM/Core/ShaderPipeline.h>
#include <string>
#include <URM/Core/Material.h>

namespace URM::Engine {

	// PLAN: Try adding support for custom shaders with different vertex types.
	// TODO: Add support for custom PixelConstantBuffer types.
	class ModelObject : public SceneObject {
		// TODO: Move defaults, VertexConstantBuffer and PixelConstantBuffer to a separate class.
		static std::shared_ptr<Core::Material> mDefaultMaterial;
		static std::shared_ptr<Core::VertexShader> mDefaultVertexShader;
		static std::shared_ptr<Core::ModelLoaderLayout> mDefaultInputLayout;

		std::shared_ptr<Core::Material> mMaterial = nullptr;
		std::shared_ptr<Core::VertexShader> mVertexShader = nullptr;
		std::shared_ptr<Core::ModelLoaderLayout> mInputLayout = nullptr;

		std::string mPath;

		void AddMeshRecursive(const std::shared_ptr<Core::ModelLoaderNode>& node, const std::weak_ptr<SceneObject>& parent);
	public:
		static std::shared_ptr<Core::Material> GetDefaultMaterial(Core::D3DCore& core);
		static std::shared_ptr<Core::VertexShader> GetDefaultVertexShader(Core::D3DCore& core);
		static std::shared_ptr<Core::ModelLoaderLayout> GetDefaultInputLayout(Core::D3DCore& core);

		void OnAdded() override;

		ModelObject(const std::string& path, const std::shared_ptr<Core::Material>& material, const std::shared_ptr<Core::VertexShader>& vertexShader, const std::shared_ptr<Core::ModelLoaderLayout>& layout);
		ModelObject(const std::string& path);
	};
}
