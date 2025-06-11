#include "pch.h"
#include "CameraObject.h"

#include "Scene.h"

bool URM::Engine::CameraObject::IsMainCamera() const {
	auto mainCamera = this->GetScene().GetMainCamera();
	if (mainCamera.expired()) {
		return false;
	}

	return mainCamera.lock().get() == this;
}

void URM::Engine::CameraObject::SetAsMainCamera() {
	const auto selfPtr = this->GetSelfPtr().lock();
	const auto cameraSelfPtr = std::dynamic_pointer_cast<CameraObject>(selfPtr);
	
	this->GetScene().SetMainCamera(cameraSelfPtr);
}

Matrix URM::Engine::CameraObject::CalculateProjectionMatrix(Core::Size2i viewSize) const {
	return Matrix::CreatePerspectiveFieldOfView(
		this->mFov,
		static_cast<float>(viewSize.width) / static_cast<float>(viewSize.height),
		this->mNearPlane,
		this->mFarPlane
	);
}

Matrix URM::Engine::CameraObject::CalculateViewMatrix() {
	return this->GetTransform().GetWorldSpaceMatrix().Invert();
}

URM::Engine::CameraObject::CameraObject(float fov, float nearPlane, float farPlane) {
	this->mFov = fov;
	this->mNearPlane = nearPlane;
	this->mFarPlane = farPlane;
}
