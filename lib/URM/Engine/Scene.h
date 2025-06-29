#pragma once

#include "SceneObject.h"
#include "AssetManager.h"
#include <vector>
#include <memory>
#include <URM/Core/D3DCore.h>
#include <URM/Core/Utils.h>

#include "CameraObject.h"
#include "LightObject.h"

namespace URM::Engine {
	class Scene : NonCopyable {
		friend class MeshObject;
		friend class LightObject;
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
		std::vector<std::weak_ptr<MeshObject>> mMeshes;
		std::vector<std::weak_ptr<LightObject>> mLights;

		AssetManager mAssetManager;
		std::reference_wrapper<Core::D3DCore> mCore;
	public:
		void PrintObjectsHierarchy() const;

		std::weak_ptr<CameraObject> GetMainCamera();
		void SetMainCamera(const std::weak_ptr<CameraObject>& camera);
		
		std::vector<std::weak_ptr<LightObject>>& GetLights();
		std::vector<std::weak_ptr<MeshObject>>& GetMeshes();

		AssetManager& GetAssetManager() {
			return this->mAssetManager;
		}

		Core::D3DCore& GetCore() const {
			return this->mCore;
		}

		std::weak_ptr<SceneObject> GetRoot() const {
			return this->mRootObject;
		}

		Scene(Core::D3DCore& core) :  mAssetManager(*this), mCore(core) {
			this->mRootObject = std::make_shared<SceneObject>();
			this->mRootObject->mScene = *this;
			this->mRootObject->mSelf = this->mRootObject;
		}
	};
}
