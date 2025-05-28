#pragma once

#include "SceneObject.h"
#include <URM/Core/ModelLoader.h>
#include <URM/Core/D3DInputLayout.h>
#include <URM/Core/ShaderProgram.h>
#include <URM/Core/VertexConcept.h>
#include <string>

namespace URM::Engine {

	// PLAN: Try adding support for custom shaders with different vertex types.
	// TODO: Add support for custom PixelConstantBuffer types.
	class SceneModel : public SceneObject {
		// TODO: Move defaults, VertexConstantBuffer and PixelConstantBuffer to a separate class.
		static std::shared_ptr<URM::Core::ShaderProgram> DefaultShaderProgram;
		static std::shared_ptr<URM::Core::ModelLoaderLayout> DefaultInputLayout;

		std::shared_ptr<URM::Core::ShaderProgram> shader = nullptr;
		std::shared_ptr<URM::Core::ModelLoaderLayout> inputLayout = nullptr;

		std::string path;

		void AddMeshRecursive(std::shared_ptr<URM::Core::ModelLoaderNode> node, std::weak_ptr<SceneObject> parent);
	public:
		static std::shared_ptr<URM::Core::ShaderProgram> GetDefaultShader(URM::Core::D3DCore& core) {
			if (!DefaultShaderProgram) {
				DefaultShaderProgram = std::make_shared<URM::Core::ShaderProgram>(core, L"SimpleVertexShader.cso", L"SimplePixelShader.cso");
			}

			return DefaultShaderProgram;
		}

		static std::shared_ptr<URM::Core::ModelLoaderLayout> GetDefaultInputLayout(URM::Core::D3DCore& core) {
			if (!DefaultInputLayout) {
				auto defaultShader = GetDefaultShader(core);
				DefaultInputLayout = std::make_shared<URM::Core::ModelLoaderLayout>(core, *(defaultShader.get()));
			}

			return DefaultInputLayout;
		}

		void OnAdded() override;

		SceneModel(std::string path, std::shared_ptr<URM::Core::ShaderProgram> shader, std::shared_ptr<URM::Core::ModelLoaderLayout> layout);
		SceneModel(std::string path);
	};
}