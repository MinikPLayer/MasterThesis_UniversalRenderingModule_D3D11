#include "pch.h"
#include "Transform.h"
#include "SceneObject.h"
#include <URM/Core/Utils.h>
#include <sstream>
#include <DirectXMath.h>

using namespace DirectX;

Matrix Transform::CalculateLocalModelMatrix() {
	auto matTranslation = Matrix::CreateTranslation(this->localPosition);
	auto matRotation = Matrix::CreateFromQuaternion(this->localRotation);
	auto matScaling = Matrix::CreateScale(this->localScale.x, this->localScale.y, this->localScale.z);

	return Matrix((XMMATRIX)matScaling * (XMMATRIX)matRotation * (XMMATRIX)matTranslation);
}

void Transform::UpdateMatrix(Matrix localMatrix, bool updateLocalValues) {
	if (updateLocalValues) {
		auto decomp = localMatrix.Decompose(this->localScale, this->localRotation, this->localPosition);
	}

	auto parentMatrix = (sceneObject.HasParent()) ? sceneObject.GetParent().lock()->GetTransform().GetWorldSpaceMatrix() : Matrix::Identity;
	worldSpaceModelMatrix = Matrix((XMMATRIX)localMatrix * (XMMATRIX)parentMatrix);

	// TODO: Find a faster way to do this? 
	// Decompose is probably not the fastest way to do this
	// But should work for now
	worldSpaceModelMatrix.Decompose(this->globalScale, this->globalRotation, this->globalPosition);

	for (auto child : sceneObject.GetChildren()) {
		child->GetTransform().UpdateMatrix();
	}
}

void Transform::UpdateMatrix() {
	this->UpdateMatrix(CalculateLocalModelMatrix(), false);
}

Transform::Transform(SceneObject& sceneObject) : sceneObject(sceneObject) {}

std::string Vector3ToString(Vector3 v) {
	std::stringstream ss;
	ss << "Vector3(" << v.x << ", " << v.y << ", " << v.z << ")";
	return ss.str();
}

std::string Transform::ToString() {
	std::stringstream ss;
	ss << "Transform[Position: ";
	ss << Vector3ToString(this->globalPosition);
	ss << ", Rotation: ";
	ss << Vector3ToString(this->globalRotation.ToEuler());
	ss << ", Scale: ";
	ss << Vector3ToString(this->globalScale);
	ss << "]";
	return ss.str();
}

// TODO: Test if matrix * parentInverse is the correct order!
void Transform::SetWorldSpaceMatrix(Matrix matrix) {
	auto parentMatrix = (sceneObject.HasParent()) ? sceneObject.GetParent().lock()->GetTransform().GetWorldSpaceMatrix() : Matrix::Identity;
	auto parentInverse = parentMatrix.Invert();
	auto localMatrix = Matrix((XMMATRIX)matrix * (XMMATRIX)parentInverse);
	localMatrix.Decompose(this->localScale, this->localRotation, this->localPosition);

	UpdateMatrix();
}

Matrix Transform::GetWorldSpaceMatrix() {
	return worldSpaceModelMatrix;
}

// TODO: Optimize to avoid decomposing matrix every time
void Transform::SetPosition(Vector3 position) {
	if (sceneObject.HasParent()) {
		auto parentMatrix = sceneObject.GetParent().lock()->GetTransform().GetWorldSpaceMatrix();
		auto parentInverse = parentMatrix.Invert();
		auto localPosition = Matrix::CreateTranslation(position);
		
		auto relativePosition = Matrix((XMMATRIX)localPosition * (XMMATRIX)parentInverse);
		this->localPosition = relativePosition.Translation();
	}
	else {
		this->localPosition = position;
	}

	UpdateMatrix();
}

// TODO: Fix incorrect behaviour
//void Transform::set_scale(Vector3<float> scale) {
//	if (gameObject.HasParent()) {
//		auto parentMatrix = gameObject.GetParent().lock()->GetTransform().GetWorldSpaceMatrix();
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
	if (sceneObject.HasParent()) {
		auto parentMatrix = sceneObject.GetParent().lock()->GetTransform().GetWorldSpaceMatrix();
		auto parentInverse = parentMatrix.Invert();
		auto localRotation = Matrix::CreateFromQuaternion(quat);
		auto relativeRotation = Matrix((XMMATRIX)localRotation * (XMMATRIX)parentInverse);

		Vector3 _;
		Quaternion rotation;
		relativeRotation.Decompose(_, rotation, _);
		this->localRotation = rotation;
	}
	else {
		this->localRotation = quat;
	}

	UpdateMatrix();
}

void Transform::SetLocalPosition(Vector3 position) {
	this->localPosition = position;

	UpdateMatrix();
}

void Transform::SetLocalScale(Vector3 scale) {
	this->localScale = scale;

	UpdateMatrix();
}

void Transform::SetLocalRotation(Quaternion quat) {
	this->localRotation = quat;

	UpdateMatrix();
}

Vector3 Transform::GetForwardVector() {
	return Vector3::Transform(Vector3::Forward, this->globalRotation);
}

Vector3 Transform::GetUpVector() {
	return Vector3::Transform(Vector3::Up, this->globalRotation);
}

Vector3 Transform::GetRightVector() {
	return Vector3::Transform(Vector3::Right, this->globalRotation);
}

Vector3 Transform::GetPosition() {
	return this->globalPosition;
}

Vector3 Transform::GetScale() {
	return this->globalScale;
}

Quaternion Transform::GetRotation() {
	return this->globalRotation;
}

Vector3 Transform::GetLocalPosition() {
	return this->localPosition;
}

Vector3 Transform::GetLocalScale() {
	return this->localScale;
}

Quaternion Transform::GetLocalRotation() {
	return this->localRotation;
}

SceneObject& Transform::GetSceneObject() {
	return this->sceneObject;
}