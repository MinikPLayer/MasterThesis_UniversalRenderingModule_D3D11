#pragma once

#include <URM/Core/Utils.h>
#include <URM/Scene/SceneObject.h>
#include <directxtk/SimpleMath.h>

namespace URM::Engine {
	class CameraObject : public URM::Scene::SceneObject {
		float fov = 60.0f;
		float nearPlane = 0.1f;
		float farPlane = 1000.0f;
	public:
		float GetFOV() {
			return this->fov;
		}

		void SetFOV(float newFov) {
			this->fov = newFov;
		}

		float GetNearPlane() {
			return this->nearPlane;
		}

		void SetNearPlane(float newNearPlane) {
			this->nearPlane = newNearPlane;
		}

		float GetFarPlane() {
			return this->farPlane;
		}

		void SetFarPlane(float newFarPlane) {
			this->farPlane = newFarPlane;
		}

		bool IsMainCamera();
		void SetAsMainCamera();

		// PLAN: Add caching
		DirectX::SimpleMath::Matrix CalculateProjectionMatrix(URM::Core::Size2i viewSize);
		DirectX::SimpleMath::Matrix CalculateViewMatrix();

		CameraObject(float fov = 60.0f, float nearPlane = 0.1f, float farPlane = 1000.0f);
	};
}