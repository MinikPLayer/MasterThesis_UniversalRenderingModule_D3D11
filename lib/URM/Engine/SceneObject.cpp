#include "pch.h"
#include "SceneObject.h"

namespace URM::Engine {
	void SceneObject::Destroy(std::shared_ptr<SceneObject> object) {
#if !NDEBUG
		if (object->isDestroyed) {
			spdlog::error("Object destoyed twice");
		}
#endif

		if (!object->parent.expired()) {
			object->parent.lock()->RemoveChild(object);
		}

		object->isDestroyed = true;
		for (auto child : object->children) {
			Destroy(child);
		}
	}


	std::weak_ptr<SceneObject> SceneObject::GetSelfPtr() {
#if SC_FATAL_ON
		if (self.lock() == nullptr) {
			ELOG_FATAL("Self pointer invalid. This could happen if get_self_ptr is called from the constructor. If that's the case, try calling it from the start() method");
		}
#endif

		return self;
	}

	void SceneObject::RemoveParent() {
		if (HasParent()) {
			parent.lock()->RemoveChild(self);
		}

		_hasParent = false;
		parent.reset();
	}

	void SceneObject::SetParent(std::shared_ptr<SceneObject> _parent) {
		_parent->__AddChild__(this->self);
	}

	void SceneObject::__AddChild__(std::weak_ptr<SceneObject> child) {
		auto c = child.lock();
		children.push_back(c);

		c->RemoveParent();

		if (c->self.lock() == nullptr) {
			c->self = child;
		}

		c->parent = self;
		c->_hasParent = true;

		c->transform.UpdateMatrix();
	}

	void SceneObject::__PrintHierarchy__(int level) {
		std::stringstream ss;
		for (int i = 0; i < level; i++) {
			ss << "\t";
		}
		ss << typeid(*this).name();
		spdlog::info(ss.str());

		for (auto child : children) {
			child->__PrintHierarchy__(level + 1);
		}
	}

	void SceneObject::RemoveChild(std::weak_ptr<SceneObject> child) {
		child.lock().get()->parent = std::weak_ptr<SceneObject>();
		for (auto it = children.begin(); it != children.end(); ++it) {
			if (*it == child.lock()) {
				children.erase(it);
				break;
			}
		}
	}

	SceneObject::~SceneObject() {
		for (auto it = children.begin(); it != children.end(); ++it) {
			(*it)->parent = std::weak_ptr<SceneObject>();
		}

		if (!isDestroyed) {
			isDestroyed = true;
		}

	}
}