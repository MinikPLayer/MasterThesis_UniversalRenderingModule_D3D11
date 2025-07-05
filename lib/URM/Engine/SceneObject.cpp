#include "pch.h"
#include "SceneObject.h"
#include "Engine.h"

namespace URM::Engine {
	void SceneObject::Destroy() {
#if !NDEBUG
		if (this->mIsDestroyed) {
			spdlog::error("Object destoyed twice");
		}
#endif

		this->RunOnDestroyed();
		if (this->HasParent()) {
			this->mParent.lock()->RemoveChild(this, false);
		}

		this->mIsDestroyed = true;

		// Copy children to avoid invalidating iterators during destruction
		auto children = this->mChildren;
		for (auto child : children) {
			child->Destroy();
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
			mParent.lock()->RemoveChild(this);
		}

		mParent.reset();
	}

	void SceneObject::SetParent(const std::shared_ptr<SceneObject>& parent) const {
		parent->AddChildInternal(this->mSelf);
	}

	void SceneObject::RunOnAdded() {
		if(!mIsOnAddedCalled) {
			mIsOnAddedCalled = true;
			this->OnAdded();
		}
	}

	void SceneObject::RunOnDestroyed() {
		if(!mIsOnDestroyedCalled) {
			mIsOnDestroyedCalled = true;
			this->OnDestroyed();
		}
	}

	void SceneObject::AddChildInternal(const std::weak_ptr<SceneObject>& child) {
		auto c = child.lock();
		mChildren.push_back(c);

		c->RemoveParent();

		if (c->mSelf.lock() == nullptr) {
			c->mSelf = child;
		}

		c->mParent = mSelf;
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

	void SceneObject::OnEngineUpdate(Engine& engine) {}

	void SceneObject::RunEventRecursively(std::function<void(SceneObject*)> event)
	{
		event(this);
	
		for (auto& child : this->mChildren) {
			child->RunEventRecursively(event);
		}
	}

	void SceneObject::RemoveChild(SceneObject* child, bool destroy) {
		child->mParent.reset();
		for (auto it = this->mChildren.begin(); it != this->mChildren.end(); ++it) {
			if ((*it).get() == child) {
				if (destroy) {
					(*it)->Destroy();
				}
				mChildren.erase(it);
				break;
			}
		}
	}

	SceneObject::~SceneObject() {
		if (!mIsDestroyed) {
			this->Destroy();
			mIsDestroyed = true;
		}

	}
}
