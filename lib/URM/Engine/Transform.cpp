
#include "pch.h"
#include "Transform.h"
#include "SceneObject.h"
#include <URM/Core/Utils.h>
#include <sstream>
#include <directxtk/SimpleMath.h>

using namespace DirectX;

// Fix weird linker issues.
const Matrix Matrix::Identity = Matrix(XMMatrixIdentity());
const Quaternion Quaternion::Identity = Quaternion(XMQuaternionIdentity());
const Vector2 Vector2::Zero = Vector2(0, 0);
const Vector3 Vector3::Zero = Vector3(0, 0, 0);
const Vector3 Vector3::One = Vector3(1, 1, 1);
const Vector3 Vector3::Up = Vector3(0, 1, 0);
const Vector3 Vector3::Right = Vector3(1, 0, 0);
const Vector3 Vector3::Forward = Vector3(0, 0, 1);

namespace URM::Engine {
	Matrix Transform::CalculateLocalModelMatrix() const {
		auto matTranslation = Matrix::CreateTranslation(this->mLocalPosition);
		auto matRotation = Matrix::CreateFromQuaternion(this->mLocalRotation);
		auto matScaling = Matrix::CreateScale(this->mLocalScale.x, this->mLocalScale.y, this->mLocalScale.z);

		return Matrix(static_cast<XMMATRIX>(matScaling) * static_cast<XMMATRIX>(matRotation) * static_cast<XMMATRIX>(matTranslation));
	}
	
	Quaternion Transform::EulerToQuatRadians(Vector3 eulerAngles) {
		return Quaternion::CreateFromYawPitchRoll(
			eulerAngles.y,
			eulerAngles.x,
			eulerAngles.z
		);
	}

	Quaternion Transform::EulerToQuatAngles(Vector3 eulerAngles) {
		return Quaternion::CreateFromYawPitchRoll(
			XMConvertToRadians(eulerAngles.y),
			XMConvertToRadians(eulerAngles.x),
			XMConvertToRadians(eulerAngles.z)
		);
	}

	// TODO: Compute inverse matrix together with a normal one
	void Transform::UpdateMatrix(Matrix localMatrix, bool updateLocalValues) {
		if (updateLocalValues) {
			localMatrix.Decompose(this->mLocalScale, this->mLocalRotation, this->mLocalPosition);
		}

		auto parentMatrix = (mSceneObject.HasParent()) ? mSceneObject.GetParent().lock()->GetTransform().GetWorldSpaceMatrix() : Matrix::Identity;
		mWorldSpaceModelMatrix = Matrix(static_cast<XMMATRIX>(localMatrix) * static_cast<XMMATRIX>(parentMatrix));

		// PLAN: Find a faster way to do this? 
		// Decompose is probably not the fastest way to do this
		// But should work for now
		mWorldSpaceModelMatrix.Decompose(this->mGlobalScale, this->mGlobalRotation, this->mGlobalPosition);

		for (auto child : mSceneObject.GetChildren()) {
			child->GetTransform().UpdateMatrix();
		}
	}

	void Transform::UpdateMatrix() {
		this->UpdateMatrix(CalculateLocalModelMatrix(), false);
	}

	Transform::Transform(SceneObject& sceneObject) : mSceneObject(sceneObject) {}

	std::string Vector3ToString(Vector3 v) {
		std::stringstream ss;
		ss << "Vector3(" << v.x << ", " << v.y << ", " << v.z << ")";
		return ss.str();
	}

	std::string Transform::ToString() const {
		std::stringstream ss;
		ss << "Transform[Position: ";
		ss << Vector3ToString(this->mGlobalPosition);
		ss << ", Rotation: ";
		ss << Vector3ToString(this->mGlobalRotation.ToEuler());
		ss << ", Scale: ";
		ss << Vector3ToString(this->mGlobalScale);
		ss << "]";
		return ss.str();
	}

	void Transform::SetWorldSpaceMatrix(const Matrix& matrix) {
		auto parentMatrix = (mSceneObject.HasParent()) ? mSceneObject.GetParent().lock()->GetTransform().GetWorldSpaceMatrix() : Matrix::Identity;
		auto parentInverse = parentMatrix.Invert();
		auto localMatrix = Matrix(static_cast<XMMATRIX>(matrix) * static_cast<XMMATRIX>(parentInverse));
		localMatrix.Decompose(this->mLocalScale, this->mLocalRotation, this->mLocalPosition);

		UpdateMatrix();
	}

	Matrix Transform::GetWorldSpaceMatrix() const {
		return mWorldSpaceModelMatrix;
	}

	// PLAN: Optimize to avoid decomposing matrix every time
	void Transform::SetPosition(Vector3 position) {
		if (mSceneObject.HasParent()) {
			auto parentMatrix = mSceneObject.GetParent().lock()->GetTransform().GetWorldSpaceMatrix();
			auto parentInverse = parentMatrix.Invert();
			auto localPosition = Matrix::CreateTranslation(position);

			auto relativePosition = Matrix(static_cast<XMMATRIX>(localPosition) * static_cast<XMMATRIX>(parentInverse));
			this->mLocalPosition = relativePosition.Translation();
		}
		else {
			this->mLocalPosition = position;
		}

		UpdateMatrix();
	}

	// PLAN: Fix incorrect behaviour
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
		if (mSceneObject.HasParent()) {
			auto parentMatrix = mSceneObject.GetParent().lock()->GetTransform().GetWorldSpaceMatrix();
			auto parentInverse = parentMatrix.Invert();
			auto localRotation = Matrix::CreateFromQuaternion(quat);
			auto relativeRotation = Matrix(static_cast<XMMATRIX>(localRotation) * static_cast<XMMATRIX>(parentInverse));

			Vector3 _;
			Quaternion rotation;
			relativeRotation.Decompose(_, rotation, _);
			this->mLocalRotation = rotation;
		}
		else {
			this->mLocalRotation = quat;
		}

		UpdateMatrix();
	}
	void Transform::SetRotation(Vector3 eulerAngles) {
		this->SetRotation(EulerToQuatAngles(eulerAngles));
	}

	// Source: https://gamedev.stackexchange.com/questions/15070/orienting-a-model-to-face-a-target
	void Transform::LookAt(Vector3 target, Vector3 upVector) {
		auto pos = GetPosition();
		auto newForward = (target - pos);
		newForward.Normalize();

		auto source = Vector3::Forward;
		auto dest = newForward;
		
		float dot = source.Dot(dest);

		if (abs(dot - (-1.0f)) < 0.000001f) {
			// Facing the opposite direction
			SetRotation(Quaternion::CreateFromAxisAngle(upVector, 180.0f));
			return;
		}

		if (abs(dot - 1.0f) < 0.000001f) {
			// Facing the target
			return;
		}

		float rotAngle = (float)acos(dot);
		auto rotAxis = source.Cross(dest);
		rotAxis.Normalize();
		SetRotation(Quaternion::CreateFromAxisAngle(rotAxis, rotAngle));
	}

	void Transform::SetLocalPosition(Vector3 position) {
		this->mLocalPosition = position;

		UpdateMatrix();
	}

	void Transform::SetLocalScale(Vector3 scale) {
		this->mLocalScale = scale;

		UpdateMatrix();
	}

	void Transform::SetLocalRotation(Quaternion quat) {
		this->mLocalRotation = quat;

		UpdateMatrix();
	}

	void Transform::SetLocalRotation(Vector3 eulerAngles) {
		this->SetLocalRotation(EulerToQuatAngles(eulerAngles));
	}

	Vector3 Transform::GetForwardVector() const {
		return Vector3::Transform(Vector3::Forward, this->mGlobalRotation);
	}

	Vector3 Transform::GetUpVector() const {
		return Vector3::Transform(Vector3::Up, this->mGlobalRotation);
	}

	Vector3 Transform::GetRightVector() const {
		return Vector3::Transform(Vector3::Right, this->mGlobalRotation);
	}

	Vector3 Transform::GetPosition() const {
		return this->mGlobalPosition;
	}

	Vector3 Transform::GetScale() const {
		return this->mGlobalScale;
	}

	Quaternion Transform::GetRotation() const {
		return this->mGlobalRotation;
	}

	Vector3 Transform::GetLocalPosition() const {
		return this->mLocalPosition;
	}

	Vector3 Transform::GetLocalScale() const {
		return this->mLocalScale;
	}

	Quaternion Transform::GetLocalRotation() const {
		return this->mLocalRotation;
	}

	SceneObject& Transform::GetSceneObject() const {
		return this->mSceneObject;
	}
}
