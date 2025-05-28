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
		std::shared_ptr<URM::Core::Mesh<URM::Core::ModelLoaderVertexType>> mesh,
		std::shared_ptr<URM::Core::D3DInputLayout<URM::Core::ModelLoaderVertexType>> inputLayout,
		std::shared_ptr<URM::Core::ShaderProgram> shader
	) : mesh(mesh), inputLayout(inputLayout), shader(shader) {}
}
