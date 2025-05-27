#pragma once

#include <memory>
#include <URM/Core/pch.h>

namespace URM::Scene {
	class SceneObject;
	class Transform {
		Matrix worldSpaceModelMatrix = Matrix::Identity;
		SceneObject& sceneObject;

		Vector3 localPosition = Vector3(0.0f, 0.0f, 0.0f);
		Quaternion localRotation = Quaternion::Identity;
		Vector3 localScale = Vector3::One;

		Vector3 globalPosition;
		Quaternion globalRotation;
		Vector3 globalScale;

		Matrix CalculateLocalModelMatrix();

	public:
		void UpdateMatrix();
		void UpdateMatrix(Matrix localMatrix, bool updateLocalValues = true);

		void SetWorldSpaceMatrix(Matrix matrix);
		Matrix GetWorldSpaceMatrix();

		void SetPosition(Vector3 position);
		void SetRotation(Quaternion quat);

		void SetLocalPosition(Vector3 position);
		void SetLocalScale(Vector3 scale);
		void SetLocalRotation(Quaternion quat);

		Vector3 GetForwardVector();
		Vector3 GetUpVector();
		Vector3 GetRightVector();

		Vector3 GetPosition();
		Vector3 GetScale();
		Quaternion GetRotation();

		Vector3 GetLocalPosition();
		Vector3 GetLocalScale();
		Quaternion GetLocalRotation();

		SceneObject& GetSceneObject();

		// Disable copy constructor and assignment operator
		Transform(const Transform&) = delete;
		Transform& operator=(const Transform&) = delete;

		Transform(SceneObject& sceneObject);

		std::string ToString();
	};
}