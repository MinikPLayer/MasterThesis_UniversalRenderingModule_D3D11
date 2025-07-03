#pragma once

#include "SceneObject.h"
#include <URM/Core/ModelLoader.h>
#include <URM/Core/D3DInputLayout.h>
#include <URM/Core/ShaderPipeline.h>
#include <string>

namespace URM::Engine {

	// PLAN: Try adding support for custom shaders with different vertex types.
	// TODO: Add support for custom PixelConstantBuffer types.
	class ModelObject : public SceneObject {
		// TODO: Move defaults, VertexConstantBuffer and PixelConstantBuffer to a separate class.
		static std::map<URM::Core::RenderingStage, std::shared_ptr<Core::ShaderPipeline>> mDefaultShaders;
		static std::map<URM::Core::RenderingStage, std::shared_ptr<Core::ModelLoaderLayout>> mDefaultInputLayouts;

		std::map<URM::Core::RenderingStage, std::shared_ptr<Core::ShaderPipeline>> mShaders;
		std::map<URM::Core::RenderingStage, std::shared_ptr<Core::ModelLoaderLayout>> mInputLayouts;

		std::string mPath;

		void AddMeshRecursive(const std::shared_ptr<Core::ModelLoaderNode>& node, const std::weak_ptr<SceneObject>& parent);
	public:
		static std::shared_ptr<Core::ShaderPipeline> GetDefaultShader(Core::D3DCore& core, URM::Core::RenderingStage stage);
		static std::shared_ptr<Core::ModelLoaderLayout> GetDefaultInputLayout(Core::D3DCore& core, URM::Core::RenderingStage stage);

		void OnAdded() override;

		ModelObject(const std::string& path, const std::map<URM::Core::RenderingStage, std::shared_ptr<Core::ShaderPipeline>> shaders, const std::map<URM::Core::RenderingStage, std::shared_ptr<Core::ModelLoaderLayout>> inputLayouts);
		ModelObject(const std::string& path);
	};
}
