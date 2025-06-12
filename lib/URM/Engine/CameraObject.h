#pragma once

#include <URM/Core/Utils.h>
#include <directxtk/SimpleMath.h>

#include "SceneObject.h"

namespace URM::Engine {
	class CameraObject : public SceneObject {
		// Field of view in degrees
		float mFov = 60.0f;

		// Near cutoff plane
		float mNearPlane = 0.1f;

		// Far cutoff plane
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
		
		// PLAN: Add caching
		virtual Matrix CalculateProjectionMatrix(Core::Size2i viewSize) const;
		virtual Matrix CalculateViewMatrix();

		CameraObject(float fov = 60.0f, float nearPlane = 0.1f, float farPlane = 1000.0f);
	};
}
