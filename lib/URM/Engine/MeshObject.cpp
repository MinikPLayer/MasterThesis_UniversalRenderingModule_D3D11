#include "pch.h"
#include "MeshObject.h"
#include "Scene.h"

namespace URM::Engine {
	void MeshObject::OnAdded() {
		auto& scene = GetScene();

		auto selfPtr = std::static_pointer_cast<MeshObject>(this->GetSelfPtr().lock());
		scene.mMeshes.push_back(selfPtr);
	}

	void MeshObject::OnDestroyed() {
		auto thisPtr = this->GetSelfPtr().lock();

		auto& scene = GetScene();
		for (size_t i = 0; i < scene.mMeshes.size(); i++) {
			if (scene.mMeshes[i].lock() == thisPtr) {
				scene.mMeshes.erase(scene.mMeshes.begin() + i);
				i--;
				break;
			}
		}
	}

	MeshObject::MeshObject( 
		const std::shared_ptr<Core::Mesh<Core::ModelLoaderVertexType>>& mesh,
		const std::map<URM::Core::RenderingStage, std::shared_ptr<Core::ModelLoaderLayout>> inputLayouts,
		const std::map<URM::Core::RenderingStage, std::shared_ptr<Core::ShaderPipeline>> shaders
	) : mMesh(mesh), mInputLayouts(inputLayouts), mShaders(shaders) {}
}
