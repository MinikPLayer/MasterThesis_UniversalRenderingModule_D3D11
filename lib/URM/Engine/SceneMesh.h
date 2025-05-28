#pragma once

#include "SceneObject.h"
#include <URM/Core/ModelLoader.h>
#include <URM/Core/D3DInputLayout.h>
#include <URM/Core/ShaderProgram.h>

namespace URM::Engine {
	class SceneMesh : public SceneObject {
		std::shared_ptr<Core::Mesh<Core::ModelLoaderVertexType>> mMesh;
		std::shared_ptr<Core::ModelLoaderLayout> mInputLayout;
		std::shared_ptr<Core::ShaderProgram> mShader;
	public:
		void OnAdded() override;
		void OnDestroyed() override;

		std::shared_ptr<Core::ModelLoaderLayout> GetInputLayout() {
			return this->mInputLayout;
		}

		std::shared_ptr<Core::ShaderProgram> GetShader() {
			return this->mShader;
		}

		Core::Mesh<Core::ModelLoaderVertexType>& GetMesh() const {
			return *this->mMesh;
		}

		SceneMesh(
			const std::shared_ptr<Core::Mesh<Core::ModelLoaderVertexType>>& mesh,
			const std::shared_ptr<Core::D3DInputLayout<Core::ModelLoaderVertexType>>& inputLayout,
			const std::shared_ptr<Core::ShaderProgram>& shader
		);
	};
}
