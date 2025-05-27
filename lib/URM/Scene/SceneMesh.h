#pragma once

#include "SceneObject.h"
#include <URM/Core/ModelLoader.h>
#include <URM/Core/D3DInputLayout.h>
#include <URM/Core/ShaderProgram.h>

namespace URM::Scene {
	class SceneMesh : public SceneObject {
		URM::Core::Mesh<URM::Core::ModelLoaderVertexType> mesh;
		std::shared_ptr<URM::Core::ModelLoaderLayout> inputLayout;
		std::shared_ptr<URM::Core::ShaderProgram> shader;

	public:
		void OnAdded() override;
		void OnDestroyed() override;

		std::shared_ptr<URM::Core::ModelLoaderLayout> GetInputLayout() {
			return this->inputLayout;
		}

		std::shared_ptr<URM::Core::ShaderProgram> GetShader() {
			return this->shader;
		}

		URM::Core::Mesh<URM::Core::ModelLoaderVertexType> GetMesh() {
			return this->mesh;
		}

		SceneMesh(
			URM::Core::Mesh<URM::Core::ModelLoaderVertexType> mesh,
			std::shared_ptr<URM::Core::D3DInputLayout<URM::Core::ModelLoaderVertexType>> inputLayout,
			std::shared_ptr<URM::Core::ShaderProgram> shader
		);
	};
}