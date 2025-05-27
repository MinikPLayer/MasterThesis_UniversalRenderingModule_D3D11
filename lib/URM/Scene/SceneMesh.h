#pragma once

#include "SceneObject.h"
#include <URM/Core/ModelLoader.h>

namespace URM::Scene {
	class SceneMesh : public SceneObject {
		URM::Core::Mesh<URM::Core::ModelLoaderVertexType> mesh;

	public:
		void OnAdded() override;
		void OnDestroyed() override;

		URM::Core::Mesh<URM::Core::ModelLoaderVertexType> GetMesh() {
			return this->mesh;
		}

		SceneMesh(URM::Core::Mesh<URM::Core::ModelLoaderVertexType> mesh);
	};
}