#include "pch.h"
#include "CameraObject.h"

#include "Scene.h"

bool URM::Engine::CameraObject::IsMainCamera() {
    return false;
}

void URM::Engine::CameraObject::SetAsMainCamera() {
}

DirectX::SimpleMath::Matrix URM::Engine::CameraObject::CalculateProjectionMatrix(URM::Core::Size2i viewSize) {
    return Matrix::CreatePerspectiveFieldOfView(
        this->fov,
        (float)viewSize.width / (float)viewSize.height,
        this->nearPlane,
        this->farPlane
    );
}

DirectX::SimpleMath::Matrix URM::Engine::CameraObject::CalculateViewMatrix() {
    return this->GetTransform().GetWorldSpaceMatrix().Invert();
}

URM::Engine::CameraObject::CameraObject(float fov, float nearPlane, float farPlane) {
    this->fov = fov;
    this->nearPlane = nearPlane;
    this->farPlane = farPlane;
}
