#include "pch.h"
#include "CameraObject.h"

#include "Scene.h"

Matrix URM::Engine::CameraObject::CalculateProjectionMatrix(Core::Size2i viewSize) const {
	auto fovInRadians = DirectX::XMConvertToRadians(this->mFov);
	return DirectX::XMMatrixPerspectiveFovLH(
		fovInRadians,
		static_cast<float>(viewSize.width) / static_cast<float>(viewSize.height),
		this->mNearPlane,
		this->mFarPlane
	);
}

Matrix URM::Engine::CameraObject::CalculateViewMatrix() {
	auto& transform = this->GetTransform();
	//return this->GetTransform().GetWorldSpaceMatrix().Invert();

	return DirectX::XMMatrixLookToLH(
		transform.GetPosition(),
		transform.GetForwardVector(),
		transform.GetUpVector()
	);
	// return DirectX::XMMatrixLookAtRH(transform.GetPosition(), Vector3(0, 0, 0), Vector3::Up);
}

URM::Engine::CameraObject::CameraObject(float fov, float nearPlane, float farPlane) {
	this->mFov = fov;
	this->mNearPlane = nearPlane;
	this->mFarPlane = farPlane;
}
