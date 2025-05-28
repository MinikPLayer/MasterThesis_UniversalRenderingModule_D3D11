#pragma once

#include <URM/Core/pch.h>

namespace URM::Engine {
	class SceneObject;
	class Transform {
		Matrix mWorldSpaceModelMatrix = Matrix::Identity;
		SceneObject& mSceneObject;

		Vector3 mLocalPosition = Vector3(0.0f, 0.0f, 0.0f);
		Quaternion mLocalRotation = Quaternion::Identity;
		Vector3 mLocalScale = Vector3::One;

		Vector3 mGlobalPosition;
		Quaternion mGlobalRotation;
		Vector3 mGlobalScale;

		Matrix CalculateLocalModelMatrix() const;
	public:
		void UpdateMatrix();
		void UpdateMatrix(Matrix localMatrix, bool updateLocalValues = true);

		void SetWorldSpaceMatrix(const Matrix& matrix);
		Matrix GetWorldSpaceMatrix() const;

		void SetPosition(Vector3 position);
		void SetRotation(Quaternion quat);

		void SetLocalPosition(Vector3 position);
		void SetLocalScale(Vector3 scale);
		void SetLocalRotation(Quaternion quat);

		Vector3 GetForwardVector() const;
		Vector3 GetUpVector() const;
		Vector3 GetRightVector() const;

		Vector3 GetPosition() const;
		Vector3 GetScale() const;
		Quaternion GetRotation() const;

		Vector3 GetLocalPosition() const;
		Vector3 GetLocalScale() const;
		Quaternion GetLocalRotation() const;

		SceneObject& GetSceneObject() const;

		// Disable copy constructor and assignment operator
		Transform(const Transform&) = delete;
		Transform& operator=(const Transform&) = delete;

		Transform(SceneObject& sceneObject);

		std::string ToString() const;
	};
}
