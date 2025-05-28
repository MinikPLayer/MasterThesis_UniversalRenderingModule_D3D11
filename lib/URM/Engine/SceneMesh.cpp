#include "pch.h"
#include "SceneMesh.h"
#include "Scene.h"

namespace URM::Engine {
	void SceneMesh::OnAdded() {
		auto& scene = GetScene();

		auto selfPtr = std::static_pointer_cast<SceneMesh>(this->GetSelfPtr().lock());
		scene.meshes.push_back(selfPtr);
	}

	void SceneMesh::OnDestroyed() {
		auto thisPtr = this->GetSelfPtr().lock();

		auto& scene = GetScene();
		for (size_t i = 0; i < scene.meshes.size(); i++) {
			if (scene.meshes[i].lock() == thisPtr) {
				scene.meshes.erase(scene.meshes.begin() + i);
				i--;
				break;
			}
		}
	}

	SceneMesh::SceneMesh(
		const std::shared_ptr<Core::Mesh<Core::ModelLoaderVertexType>>& mesh,
		const std::shared_ptr<Core::D3DInputLayout<Core::ModelLoaderVertexType>>& inputLayout,
		const std::shared_ptr<Core::ShaderProgram>& shader
	) : mMesh(mesh), mInputLayout(inputLayout), mShader(shader) {}
}
