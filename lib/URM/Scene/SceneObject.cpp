#include "pch.h"
#include "SceneObject.h"
#include "Scene.h"

Scene& SceneObject::GetScene() {
	return scene;
}

Transform& SceneObject::GetTransform() {
	return transform;
}

bool SceneObject::HasParent() {
	return !parent.expired();
}

std::weak_ptr<SceneObject> SceneObject::GetParent() const {
	return this->parent;
}

void SceneObject::SetParent(std::weak_ptr<SceneObject> parent) {
	this->parent = parent;

	this->transform.UpdateMatrix();
}

void SceneObject::RemoveChild(std::shared_ptr<SceneObject> child) {
	auto it = std::find(this->children.begin(), this->children.end(), child);
	if (it != this->children.end()) {
		this->children.erase(it);
		child->SetParent(std::weak_ptr<SceneObject>()); // Clear the parent of the child
	}
	else {
		spdlog::warn("Child object {} not found in parent {}'s children.", child->GetName(), this->GetName());
	}
}

void SceneObject::AddChild(std::shared_ptr<SceneObject> child) {
	if (child->HasParent()) {
		spdlog::warn("Child object {} already has a parent, removing it from the old parent.", child->GetName());
		child->GetParent().lock()->RemoveChild(child);
	}
	child->SetParent(shared_from_this());
	this->children.push_back(child);
}

std::vector<std::shared_ptr<SceneObject>> SceneObject::GetChildren() const {
	return this->children;
}

std::string SceneObject::GetName() const {
	return this->name;
}

void SceneObject::SetName(const std::string& name) {
	this->name = name;
}

SceneObject::SceneObject(Scene& scene, std::string name) : transform(*this), scene(scene) {
	this->name = name;
}

