#pragma once

#include <Core/pch.h>
#include <string>
#include "Transform.h"

class Scene;
class SceneObject {
	Scene& scene;

	Transform transform;

	std::string name;

	std::weak_ptr<SceneObject> parent;
	std::vector<std::shared_ptr<SceneObject>> children;

protected:
	Scene& GetScene();

public:
	Transform& GetTransform();
	bool HasParent();

	std::weak_ptr<SceneObject> GetParent() const;
	void SetParent(std::weak_ptr<SceneObject> parent);

	void RemoveChild(std::shared_ptr<SceneObject> child);
	void AddChild(std::shared_ptr<SceneObject> child);
	std::vector<std::shared_ptr<SceneObject>>& GetChildren() const;

	std::string GetName() const;
	void SetName(const std::string& name);

	SceneObject(Scene& scene, std::string name);
};