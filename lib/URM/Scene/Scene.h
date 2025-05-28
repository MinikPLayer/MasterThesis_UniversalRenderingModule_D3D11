#pragma once

#include "SceneObject.h"
#include "AssetManager.h"
#include <vector>
#include <memory>
#include <URM/Core/D3DCore.h>
#include <URM/Core/Utils.h>

namespace URM::Scene {
	class Scene {
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

		virtual CustomData GetCustomData_Internal() {
			return CustomData(nullptr, 0);
		}
	public:
		template<typename T>
		T* GetCustomData() {
			auto internalData = GetCustomData_Internal();
			if (internalData.data == nullptr) {
				return nullptr;
			}

			if (!URM::Core::TypeUtils::IsType<T>(internalData.typeCode)) {
				throw std::runtime_error(
					fmt::format(
						"Scene custom data type mismatch! Expected: {}, got: {}", 
						URM::Core::TypeUtils::GetTypeCode<T>(),
						internalData.typeCode
					)
				);
			}

			return (T*)internalData.data;
		}

		// Disable copy constructor and assignment operator
		Scene(const Scene&) = delete;
		Scene& operator=(const Scene&) = delete;

		std::vector<std::weak_ptr<SceneMesh>>& GetMeshes();

		void PrintObjectsHierarchy();

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