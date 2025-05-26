#pragma once

#include <string>

class SceneObject {
	std::string name;

public:
	std::string GetName() const {
		return name;
	}

	void SetName(const std::string& name) {
		this->name = name;
	}

	SceneObject(std::string name) {
		this->name = name;
	}
};