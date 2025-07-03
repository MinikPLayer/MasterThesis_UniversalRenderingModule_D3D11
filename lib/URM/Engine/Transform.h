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
		static Quaternion EulerToQuatRadians(Vector3 eulerAngles);
		static Quaternion EulerToQuatAngles(Vector3 eulerAngles);

		void UpdateMatrix();
		void UpdateMatrix(Matrix localMatrix, bool updateLocalValues = true);

		void SetWorldSpaceMatrix(const Matrix& matrix);
		Matrix GetWorldSpaceMatrix() const;

		void SetPosition(Vector3 position);
		void SetRotation(Quaternion quat);

		/// <summary>
		/// Sets the rotation using the specified Euler angles.
		/// </summary>
		/// <param name="eulerAngles">A Vector3 representing the rotation angles (in degrees or radians, depending on context) around the X, Y, and Z axes.</param>
		void SetRotation(Vector3 eulerAngles);

		void LookAt(Vector3 target, Vector3 upVector = Vector3::Up);
		void LookAtFast(Vector3 target, Vector3 upVector = Vector3::Up);
		
		void SetLocalPosition(Vector3 position);
		void SetLocalScale(Vector3 scale);
		void SetLocalRotation(Quaternion quat);

		/// <summary>
		/// Sets the local rotation of the transform using Euler angles in degrees.
		/// </summary>
		/// <param name="eulerAngles">Angles in degrees.</param>
		void SetLocalRotation(Vector3 eulerAngles);

		Vector3 GetForwardVector() const;
		Vector3 GetUpVector() const;
		Vector3 GetRightVector() const;

		Vector3 GetLocalForwardVector() const;
		Vector3 GetLocalUpVector() const;
		Vector3 GetLocalRightVector() const;

		Vector3 GetPosition() const;
		Vector3 GetScale() const;
		Quaternion GetRotation() const;

		/// <summary>
		/// Returns the rotation of the object as Euler angles.
		/// </summary>
		/// <returns>A Vector3 representing the rotation in Euler angles (pitch, yaw, roll).</returns>
		Vector3 GetRotationEuler() const;

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
