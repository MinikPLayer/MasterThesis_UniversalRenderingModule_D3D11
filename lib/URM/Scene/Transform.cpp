#include "Transform.h"
#include "SceneObject.h"
#include <Core/Utils.h>

using namespace DirectX;

XMMATRIX Transform::CalculateLocalModelMatrix() {
	auto matTranslation = XMMatrixTranslation(this->localPosition.x, this->localPosition.y, this->localPosition.z);
	auto matRotation = this->localRotation.rotation_matrix();
	auto matScaling = XMMatrixScaling(this->localScale.x, this->localScale.y, this->localScale.z);

	return matScaling * matRotation * matTranslation;
}

void Transform::UpdateMatrix(XMMATRIX localMatrix, bool updateLocalValues) {
	if (updateLocalValues) {
		auto decomp = MatrixUtils::DecomposeMatrix(localMatrix);
		this->localPosition = Vector3<float>(decomp.translation);
		this->localScale = Vector3<float>(decomp.scale);
		this->localRotation = Quaternion(decomp.rotation);
	}

	auto parentMatrix = (sceneObject.has_parent()) ? sceneObject.get_parent().lock()->get_transform().GetWorldSpaceMatrix() : XMMatrixIdentity();
	worldSpaceModelMatrix = parentMatrix * localMatrix;

	// TODO: Find a faster way to do this? 
	// Decompose is probably not the fastest way to do this
	// But should work for now
	auto dec = MatrixUtils::DecomposeMatrix(worldSpaceModelMatrix);

	this->globalPosition = Vector3<float>(dec.translation);
	this->globalScale = Vector3<float>(dec.scale);
	this->globalRotation = Quaternion(dec.rotation);

	for (auto child : sceneObject.get_children()) {
		child->get_transform().update_matrix();
	}
}

void Transform::UpdateMatrix() {
	UpdateMatrix(CalculateLocalModelMatrix(), false);
}

Transform::Transform(SceneObject& sceneObject) : sceneObject(sceneObject) {}

std::string Transform::ToString() {
	std::stringstream ss;
	ss << "Transform[Position: ";
	ss << this->globalPosition.ToString();
	ss << ", Rotation: ";
	ss << this->globalRotation.to_euler().ToString();
	ss << ", Scale: ";
	ss << this->globalScale.ToString();
	ss << "]";
	return ss.str();
}

void Transform::SetWorldSpaceMatrix(XMMATRIX matrix) {
	auto parentMatrix = (sceneObject.has_parent()) ? sceneObject.get_parent().lock()->get_transform().GetWorldSpaceMatrix() : XMMatrixIdentity();
	auto parentInverse = XMMatrixInverse(nullptr, parentMatrix);
	auto localMatrix = parentInverse * matrix;
	auto decomp = MatrixUtils::DecomposeMatrix(localMatrix);
	this->localPosition = Vector3<float>(decomp.translation);
	this->localScale = Vector3<float>(decomp.scale);
	this->localRotation = Quaternion(decomp.rotation);

	UpdateMatrix();
}

XMMATRIX Transform::GetWorldSpaceMatrix() {
	return worldSpaceModelMatrix;
}

// TODO: Optimize to avoid decomposing matrix every time
void Transform::SetPosition(Vector3<float> position) {
	if (sceneObject.has_parent()) {
		auto parentMatrix = sceneObject.get_parent().lock()->get_transform().GetWorldSpaceMatrix();
		auto parentInverse = XMMatrixInverse(nullptr, parentMatrix);
		auto localPosition = XMMatrixTranslation(position.x, position.y, position.z);
		auto relativePosition = parentInverse * localPosition;
		auto decomp = MatrixUtils::DecomposeMatrix(relativePosition);
		this->localPosition = decomp.translation;
	}
	else {
		this->localPosition = position;
	}

	UpdateMatrix();
}

// TODO: Fix incorrect behaviour
//void Transform::set_scale(Vector3<float> scale) {
//	if (gameObject.has_parent()) {
//		auto parentMatrix = gameObject.get_parent().lock()->get_transform().GetWorldSpaceMatrix();
//		auto parentInverse = glm::inverse(parentMatrix);
//		auto localScale = glm::scale(glm::mat4(1.0f), scale.to_glm());
//		auto relativeScale = parentInverse * localScale;
//		auto decomp = MatrixUtils::DecomposeMatrix(relativeScale);
//		this->localScale = Vector3<float>(decomp.scale);
//	}
//	else {
//		this->localPosition = scale;
//	}
//
//	update_matrix();
//}

void Transform::SetRotation(Quaternion quat) {
	if (gameObject.has_parent()) {
		auto parentMatrix = gameObject.get_parent().lock()->get_transform().GetWorldSpaceMatrix();
		auto parentInverse = glm::inverse(parentMatrix);
		auto localRotation = quat.rotation_matrix();
		auto relativeRotation = parentInverse * localRotation;
		auto decomp = MatrixUtils::GetRotation(relativeRotation);
		this->localRotation = Quaternion(decomp);
	}
	else {
		this->localRotation = quat;
	}

	UpdateMatrix();
}

void Transform::SetLocalPosition(Vector3<float> position) {
	this->localPosition = position;

	UpdateMatrix();
}

void Transform::SetLocalScale(Vector3<float> scale)
{
}

void Transform::SetLocalScaler(Vector3<float> scale) {
	this->localScale = scale;

	UpdateMatrix();
}
void Transform::SetLocalRotation(Quaternion quat) {
	this->localRotation = quat;

	UpdateMatrix();
}

Vector3<float> Transform::GetForwardVector() {
	// TODO: Implement caching (maybe)
	return globalRotation.forward();
}

Vector3<float> Transform::GetUpVector() {
	return globalRotation.up();
}

Vector3<float> Transform::GetRightVector()
{
	return Vector3<float>();
}

Vector3<float> Transform::GetPosition() {
	return this->globalPosition;
}

Vector3<float> Transform::GetScale() {
	return this->globalScale;
}

Quaternion Transform::GetRotation() {
	return this->globalRotation;
}

Vector3<float> Transform::GetLocalPosition() {
	return this->localPosition;
}

Vector3<float> Transform::GetLocalScale() {
	return this->localScale;
}

Quaternion Transform::GetLocalRotation() {
	return this->localRotation;
}

GameObject& Transform::GetSceneObject() {
	return this->gameObject;
}