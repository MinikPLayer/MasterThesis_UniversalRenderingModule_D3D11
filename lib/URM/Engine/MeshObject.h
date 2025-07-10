#pragma once

#include "SceneObject.h"
#include <URM/Core/ModelLoader.h>
#include <URM/Core/D3DInputLayout.h>
#include <URM/Core/ShaderPipeline.h>

#include <URM/Core/StandardMaterials.h>

namespace URM::Engine {
	class MeshObject : public SceneObject {
		std::shared_ptr<Core::Mesh<Core::ModelLoaderVertexType>> mMesh;
		std::shared_ptr<Core::ModelLoaderLayout> mInputLayout;
		std::shared_ptr<Core::VertexShader> mVertexShader;
	public:
		std::shared_ptr<URM::Core::Material> material;

		void OnAdded() override;
		void OnDestroyed() override;

		std::shared_ptr<Core::VertexShader> GetVertexShader() const {
			return this->mVertexShader;
		}

		std::shared_ptr<Core::ModelLoaderLayout> GetInputLayout() {
			return this->mInputLayout;
		}

		Core::Mesh<Core::ModelLoaderVertexType>& GetMesh() const {
			return *this->mMesh;
		}

		MeshObject(
			const std::shared_ptr<Core::Mesh<Core::ModelLoaderVertexType>>& mesh,
			const std::shared_ptr<Core::VertexShader>& vertexShader,
			const std::shared_ptr<Core::D3DInputLayout<Core::ModelLoaderVertexType>>& inputLayout,
			const std::shared_ptr<URM::Core::Material> material
		);
	};
}
