#pragma once

#include "Vector3.hpp"
#include <memory>

#include "Quaternion.hpp"

class GameObject;
class Transform {
	glm::mat4 worldSpaceModelMatrix = glm::mat4(1.0f);
	GameObject& gameObject;

	Vector3<float> localPosition = Vector3<float>(0.0f, 0.0f, 0.0f);
	Quaternion localRotation = Quaternion::identity();
	Vector3<float> localScale = Vector3<float>(1.0f, 1.0f, 1.0f);

	Vector3<float> globalPosition;
	Quaternion globalRotation;
	Vector3<float> globalScale;

	glm::mat4 calculate_local_model_matrix();

public:
	void update_matrix();
	void update_matrix(glm::mat4 localMatrix, bool updateLocalValues = true);

	void set_world_space_matrix(glm::mat4 matrix);
	glm::mat4 get_world_space_matrix();

	void set_position(Vector3<float> position);
	//void set_scale(Vector3<float> scale);
	void set_rotation(Quaternion quat);

	void set_local_position(Vector3<float> position);
	void set_local_scale(Vector3<float> scale);
	void set_local_rotation(Quaternion quat);

	Vector3<float> get_forward_vector();
	Vector3<float> get_up_vector();

	Vector3<float> get_position();
	Vector3<float> get_scale();
	Quaternion get_rotation();

	Vector3<float> get_local_position();
	Vector3<float> get_local_scale();
	Quaternion get_local_rotation();

	GameObject& get_gameobject();

	// Disable copy constructor and assignment operator
	Transform(const Transform&) = delete;
	Transform& operator=(const Transform&) = delete;

	Transform(GameObject& gameObject);

	std::string to_string();
};