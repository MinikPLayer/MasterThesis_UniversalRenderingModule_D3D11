#pragma once

#include "SceneObject.h"
#include <URM/Core/ModelLoader.h>
#include <URM/Core/D3DInputLayout.h>
#include <URM/Core/ShaderProgram.h>
#include <string>

namespace URM::Engine {

	// PLAN: Try adding support for custom shaders with different vertex types.
	// TODO: Add support for custom PixelConstantBuffer types.
	class SceneModel : public SceneObject {
		// TODO: Move defaults, VertexConstantBuffer and PixelConstantBuffer to a separate class.
		static std::shared_ptr<Core::ShaderProgram> mDefaultShaderProgram;
		static std::shared_ptr<Core::ModelLoaderLayout> mDefaultInputLayout;

		std::shared_ptr<Core::ShaderProgram> mShader = nullptr;
		std::shared_ptr<Core::ModelLoaderLayout> mInputLayout = nullptr;

		std::string mPath;

		void AddMeshRecursive(const std::shared_ptr<Core::ModelLoaderNode>& node, const std::weak_ptr<SceneObject>& parent);
	public:
		static std::shared_ptr<Core::ShaderProgram> GetDefaultShader(Core::D3DCore& core) {
			if (!mDefaultShaderProgram) {
				mDefaultShaderProgram = std::make_shared<Core::ShaderProgram>(core, L"SimpleVertexShader.cso", L"SimplePixelShader.cso");
			}

			return mDefaultShaderProgram;
		}

		static std::shared_ptr<Core::ModelLoaderLayout> GetDefaultInputLayout(Core::D3DCore& core) {
			if (!mDefaultInputLayout) {
				auto defaultShader = GetDefaultShader(core);
				mDefaultInputLayout = std::make_shared<Core::ModelLoaderLayout>(core, *(defaultShader.get()));
			}

			return mDefaultInputLayout;
		}

		void OnAdded() override;

		SceneModel(const std::string& path, const std::shared_ptr<Core::ShaderProgram>& shader, const std::shared_ptr<Core::ModelLoaderLayout>& layout);
		SceneModel(const std::string& path);
	};
}
