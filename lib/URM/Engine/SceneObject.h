#pragma once
#include "Transform.h"
#include <vector>
#include <memory>
#include <functional>
#include <optional>
#include <URM/Core/Utils.h>

namespace URM::Engine {
	class Engine;
	class SceneObject : NonCopyable {
		friend class Scene;
		friend class Transform;
		
	public:
		virtual void OnAdded() {}
		virtual void OnDestroyed() {}
		virtual void OnEngineUpdate(Engine& engine);

		void RunEventRecursively(std::function<void(SceneObject*)> event);
	private:
		std::optional<std::reference_wrapper<Scene>> mScene;

		std::vector<std::shared_ptr<SceneObject>> mChildren;
		std::weak_ptr<SceneObject> mParent;

		std::weak_ptr<SceneObject> mSelf;
		size_t mTypeCode = -1;

		Transform mTransform;

		bool mIsStarted = false;
		bool mIsLateStarted = false;
		bool mIsDestroyed = false;

		void AddChildInternal(const std::weak_ptr<SceneObject>& object);
		void PrintHierarchy(int level);

		template<typename T>
		static std::shared_ptr<T> Instantiate(std::reference_wrapper<Scene> scene, std::shared_ptr<T> object, std::shared_ptr<SceneObject> parent) {
			static_assert(std::is_base_of_v<SceneObject, T>, "T must derive from GameObject");

			object->mSelf = object;
			object->mTypeCode = Core::TypeUtils::GetTypeCode<T>();
			object->mScene = scene;
			parent->AddChildInternal(object);

			//object->OnAdded();
			object.get()->RunEventRecursively(&SceneObject::OnAdded);

			return object;
		}

		template<typename T>
		static std::shared_ptr<T> Instantiate(std::reference_wrapper<Scene> scene, T* object, std::shared_ptr<SceneObject> parent) {
			static_assert(std::is_base_of_v<SceneObject, T>, "T must derive from GameObject");

			auto objPtr = std::shared_ptr<T>(object);
			return Instantiate<T>(scene, objPtr, parent);
		}

		std::vector<std::shared_ptr<SceneObject>>& GetChildren() {
			return this->mChildren;
		}
	public:
		void Destroy();

		template<typename T>
		bool IsType() {
			return mTypeCode == Core::TypeUtils::GetTypeCode<T>();
		}

		std::weak_ptr<SceneObject> GetSelfPtr();

		void RemoveParent();
		void SetParent(const std::shared_ptr<SceneObject>& parent) const;

		bool HasParent() const {
			return !this->mParent.expired();
		}

		Scene& GetScene() const {
			if (!this->mScene.has_value()) {
				throw std::runtime_error("SceneObject is not attached to a scene");
			}

			return this->mScene.value().get();
		}

		Transform& GetTransform() {
			return this->mTransform;
		}

		std::weak_ptr<SceneObject> GetParent() {
			return this->mParent;
		}

		std::shared_ptr<SceneObject> GetChildByIndex(int index) {
			return this->mChildren[index];
		}

		template<typename T>
		std::vector<std::shared_ptr<T>> GetChildrenByType(bool recursive = false) {
			std::vector<std::shared_ptr<T>> result;
			for (const auto& child : this->mChildren) {
				if (child->IsType<T>()) {
					auto childPtr = std::dynamic_pointer_cast<T>(child);
					result.push_back(childPtr);
				}

				if (recursive) {
					auto childResult = child->GetChildrenByType<T>(true);
					result.insert(result.end(), childResult.begin(), childResult.end());
				}
			}
			return result;
		}

		const char* GetTypeName() {
			return typeid(*this).name();
		}

		template<typename T>
		std::shared_ptr<T> AddChild(std::shared_ptr<T> child) {
			if (mSelf.lock() == nullptr) {
				spdlog::warn("Trying to add component to uninitialized object");
			}

			if (!this->mScene.has_value()) {
				throw std::runtime_error("SceneObject is not attached to a scene");
			}

			return Instantiate<T>(this->mScene.value(), child, this->mSelf.lock());
		}

		template<typename T>
		std::shared_ptr<T> AddChild(T* child) {
			if (mSelf.lock() == nullptr) {
				spdlog::warn("Trying to add component to uninitialized object");
			}

			if (!this->mScene.has_value()) {
				throw std::runtime_error("SceneObject is not attached to a scene");
			}

			return Instantiate<T>(this->mScene.value(), child, this->mSelf.lock());
		}

		void RemoveChild(SceneObject* child, bool destroy = true);

		// Disable copy constructor and assignment operator
		SceneObject(const SceneObject&) = delete;
		SceneObject& operator=(const SceneObject&) = delete;

		SceneObject() : mTransform(*this) {}
		~SceneObject() override;
	};
}
