#pragma once
#include "Transform.h"
#include <vector>
#include <memory>
#include <functional>

class Scene;
class SceneObject {
	friend class Scene;

	std::weak_ptr<Scene> scene;

	std::vector<std::shared_ptr<SceneObject>> children;
	std::weak_ptr<SceneObject> parent;
	bool _hasParent = false;

	// TODO: This should be initialized before add component
	std::weak_ptr<SceneObject> self;
	size_t hash = -1;

	Transform transform;

	bool isStarted = false;
	bool isLateStarted = false;
	bool isDestroyed = false;

	void __AddChild__(std::weak_ptr<SceneObject> object);
	void __PrintHierarchy__(int level);

	template<typename T>
	static std::shared_ptr<T> Instantiate(std::weak_ptr<Scene> scene, std::shared_ptr<T> object, std::shared_ptr<SceneObject> parent) {
		static_assert(std::is_base_of<SceneObject, T>::value, "T must derive from GameObject");

		object->self = object;
		object->hash = typeid(T).hash_code();
		object->scene = scene;
		parent->__AddChild__(object);

		return object;
	}

	template<typename T>
	static std::shared_ptr<T> Instantiate(std::weak_ptr<Scene> scene, T* object, std::shared_ptr<SceneObject> parent) {
		static_assert(std::is_base_of<SceneObject, T>::value, "T must derive from GameObject");

		auto objPtr = std::shared_ptr<T>(object);
		return Instantiate<T>(scene, objPtr, parent);
	}
public:

	static void Destroy(std::shared_ptr<SceneObject> object);

	template<typename T>
	bool IsType() {
		return hash == typeid(T).hash_code();
	}

	std::weak_ptr<SceneObject> GetSelfPtr();

	void RemoveParent();
	void SetParent(std::shared_ptr<SceneObject> parent);

	bool HasParent() {
		return this->_hasParent;
	}

	std::weak_ptr<Scene> GetScene() {
		return this->scene;
	}

	Transform& GetTransform() {
		return this->transform;
	}

	std::weak_ptr<SceneObject> GetParent() {
		return this->parent;
	}

	std::vector<std::shared_ptr<SceneObject>> GetChildren() {
		return this->children;
	}

	const char* GetTypeName() {
		return typeid(*this).name();
	}

	template<typename T>
	std::shared_ptr<T> AddChild(std::shared_ptr<T> child) {
		if (self.lock() == nullptr) {
			spdlog::warn("Trying to add component to uninitialized object");
		}

		return Instantiate<T>(this->scene, child, this->self.lock());
	}

	template<typename T>
	std::shared_ptr<T> AddChild(T* child) {
		return Instantiate<T>(child, this->self.lock());
	}

	void RemoveChild(std::weak_ptr<SceneObject> child);

	// Disable copy constructor and assignment operator
	SceneObject(const SceneObject&) = delete;
	SceneObject& operator=(const SceneObject&) = delete;

	SceneObject(SceneObject&&) = default;
	SceneObject& operator=(SceneObject&&) = default;

	SceneObject() : transform(*this) {}
	~SceneObject();
};