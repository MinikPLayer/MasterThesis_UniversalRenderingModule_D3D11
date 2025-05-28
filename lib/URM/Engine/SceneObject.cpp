#include "pch.h"
#include "SceneObject.h"

namespace URM::Engine {
	void SceneObject::Destroy(const std::shared_ptr<SceneObject>& object) {
#if !NDEBUG
		if (object->mIsDestroyed) {
			spdlog::error("Object destoyed twice");
		}
#endif

		if (!object->mParent.expired()) {
			object->mParent.lock()->RemoveChild(object);
		}

		object->mIsDestroyed = true;
		for (auto child : object->mChildren) {
			Destroy(child);
		}
	}


	std::weak_ptr<SceneObject> SceneObject::GetSelfPtr() {
#if !NDEBUG
		if (mSelf.lock() == nullptr) {
			spdlog::error("Self pointer invalid. This could happen if get_self_ptr is called from the constructor. If that's the case, try calling it from the start() method");
		}
#endif

		return mSelf;
	}

	void SceneObject::RemoveParent() {
		if (HasParent()) {
			mParent.lock()->RemoveChild(mSelf);
		}

		mHasParent = false;
		mParent.reset();
	}

	void SceneObject::SetParent(const std::shared_ptr<SceneObject>& parent) const {
		parent->AddChildInternal(this->mSelf);
	}

	void SceneObject::AddChildInternal(const std::weak_ptr<SceneObject>& child) {
		auto c = child.lock();
		mChildren.push_back(c);

		c->RemoveParent();

		if (c->mSelf.lock() == nullptr) {
			c->mSelf = child;
		}

		c->mParent = mSelf;
		c->mHasParent = true;

		c->mTransform.UpdateMatrix();
	}

	void SceneObject::PrintHierarchy(int level) {
		std::stringstream ss;
		for (int i = 0; i < level; i++) {
			ss << "\t";
		}
		ss << typeid(*this).name();
		spdlog::info(ss.str());

		for (auto child : mChildren) {
			child->PrintHierarchy(level + 1);
		}
	}

	void SceneObject::RemoveChild(std::weak_ptr<SceneObject> child) {
		child.lock().get()->mParent = std::weak_ptr<SceneObject>();
		for (auto it = mChildren.begin(); it != mChildren.end(); ++it) {
			if (*it == child.lock()) {
				mChildren.erase(it);
				break;
			}
		}
	}

	SceneObject::~SceneObject() {
		for (auto it = mChildren.begin(); it != mChildren.end(); ++it) {
			(*it)->mParent = std::weak_ptr<SceneObject>();
		}

		if (!mIsDestroyed) {
			mIsDestroyed = true;
		}

	}
}
