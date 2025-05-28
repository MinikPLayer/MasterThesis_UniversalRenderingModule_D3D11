#pragma once

#include "SceneObject.h"
#include "AssetManager.h"
#include <vector>
#include <memory>
#include <URM/Core/D3DCore.h>
#include <URM/Core/Utils.h>

namespace URM::Engine {
	class Scene : NonCopyable {
		friend class SceneMesh;

	protected:
		struct CustomData {
			void* data;
			size_t typeCode;

			CustomData(void* data, size_t typeCode) {
				this->data = data;
				this->typeCode = typeCode;
			}
		};

		std::shared_ptr<SceneObject> rootObject;
		std::vector<std::weak_ptr<SceneMesh>> meshes;

		AssetManager assetManager;
		URM::Core::D3DCore& core;

	public:
		void PrintObjectsHierarchy();
		std::vector<std::weak_ptr<SceneMesh>>& GetMeshes();

		AssetManager& GetAssetManager() {
			return this->assetManager;
		}

		URM::Core::D3DCore& GetCore() {
			return this->core;
		}

		std::weak_ptr<SceneObject> GetRoot() const {
			return this->rootObject;
		}

		Scene(URM::Core::D3DCore& core) : core(core) {
			this->rootObject = std::make_shared<SceneObject>();
			this->rootObject->scene = *this;
			this->rootObject->self = this->rootObject;
		}
	};
}