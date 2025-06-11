#pragma once

#include "SceneObject.h"
#include "AssetManager.h"
#include <vector>
#include <memory>
#include <URM/Core/D3DCore.h>
#include <URM/Core/Utils.h>

#include "CameraObject.h"
#include "Light.h"

namespace URM::Engine {
	class Scene : NonCopyable {
		friend class SceneMesh;
		friend class Light;
	protected:
		struct CustomData {
			void* data;
			size_t typeCode;

			CustomData(void* data, const size_t typeCode) {
				this->data = data;
				this->typeCode = typeCode;
			}
		};

		std::weak_ptr<CameraObject> mMainCamera;
		
		std::shared_ptr<SceneObject> mRootObject;
		std::vector<std::weak_ptr<SceneMesh>> mMeshes;
		std::vector<std::weak_ptr<Light>> mLights;

		AssetManager mAssetManager;
		Core::D3DCore& mCore;
	public:
		void PrintObjectsHierarchy() const;

		std::weak_ptr<CameraObject> GetMainCamera();
		void SetMainCamera(const std::weak_ptr<CameraObject>& camera);
		
		std::vector<std::weak_ptr<Light>>& GetLights();
		std::vector<std::weak_ptr<SceneMesh>>& GetMeshes();

		AssetManager& GetAssetManager() {
			return this->mAssetManager;
		}

		Core::D3DCore& GetCore() const {
			return this->mCore;
		}

		std::weak_ptr<SceneObject> GetRoot() const {
			return this->mRootObject;
		}

		Scene(Core::D3DCore& core) : mCore(core) {
			this->mRootObject = std::make_shared<SceneObject>();
			this->mRootObject->mScene = *this;
			this->mRootObject->mSelf = this->mRootObject;
		}
	};
}
