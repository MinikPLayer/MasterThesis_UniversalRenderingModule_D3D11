#include "pch.h"
#include "Model.h"
#include <stdexcept>

Model::Model(std::string path) : SceneObject("Model TEST") {
	throw std::runtime_error("Model loading not implemented yet: " + path);
}
