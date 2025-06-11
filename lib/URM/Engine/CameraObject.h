#pragma once

#include <URM/Core/Utils.h>
#include <directxtk/SimpleMath.h>

#include "SceneObject.h"

namespace URM::Engine {
	class CameraObject : public SceneObject {
		float mFov = 60.0f;
		float mNearPlane = 0.1f;
		float mFarPlane = 1000.0f;
	public:
		float GetFov() const {
			return this->mFov;
		}

		void SetFov(float newFov) {
			this->mFov = newFov;
		}

		float GetNearPlane() const {
			return this->mNearPlane;
		}

		void SetNearPlane(float newNearPlane) {
			this->mNearPlane = newNearPlane;
		}

		float GetFarPlane() const {
			return this->mFarPlane;
		}

		void SetFarPlane(float newFarPlane) {
			this->mFarPlane = newFarPlane;
		}

		bool IsMainCamera() const;
		void SetAsMainCamera();

		// PLAN: Add caching
		Matrix CalculateProjectionMatrix(Core::Size2i viewSize) const;
		Matrix CalculateViewMatrix();

		CameraObject(float fov = 60.0f, float nearPlane = 0.1f, float farPlane = 1000.0f);
	};
}
