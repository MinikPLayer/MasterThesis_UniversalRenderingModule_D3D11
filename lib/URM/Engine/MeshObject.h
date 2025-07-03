#pragma once

#include "SceneObject.h"
#include <URM/Core/ModelLoader.h>
#include <URM/Core/D3DInputLayout.h>
#include <URM/Core/ShaderPipeline.h>

namespace URM::Engine {

	class MeshObject : public SceneObject {
		std::shared_ptr<Core::Mesh<Core::ModelLoaderVertexType>> mMesh;
		std::map<URM::Core::RenderingStage, std::shared_ptr<Core::ModelLoaderLayout>> mInputLayouts;
		std::map<URM::Core::RenderingStage, std::shared_ptr<Core::ShaderPipeline>> mShaders;
	public:
		void OnAdded() override;
		void OnDestroyed() override;

		std::shared_ptr<Core::ModelLoaderLayout> GetInputLayout(URM::Core::RenderingStage stage) {
			return this->mInputLayouts[stage];
		}

		std::shared_ptr<Core::ShaderPipeline> GetShader(URM::Core::RenderingStage stage) {
			return this->mShaders[stage];
		}

		Core::Mesh<Core::ModelLoaderVertexType>& GetMesh() const {
			return *this->mMesh;
		}

		MeshObject(
			const std::shared_ptr<Core::Mesh<Core::ModelLoaderVertexType>>& mesh,
			const std::map<URM::Core::RenderingStage, std::shared_ptr<Core::ModelLoaderLayout>> inputLayouts,
			const std::map<URM::Core::RenderingStage, std::shared_ptr<Core::ShaderPipeline>> shaders
		);
	};
}
