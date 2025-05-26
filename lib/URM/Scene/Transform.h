#pragma once

#include <Core/Vector3.h>
#include <memory>

#include <Core/Quaternion.h>
#include <DirectXMath.h>


class SceneObject;
class Transform {
	DirectX::XMMATRIX worldSpaceModelMatrix = DirectX::XMMatrixIdentity();
	SceneObject& sceneObject;

	Vector3<float> localPosition = Vector3<float>(0.0f, 0.0f, 0.0f);
	Quaternion localRotation = Quaternion::identity();
	Vector3<float> localScale = Vector3<float>(1.0f, 1.0f, 1.0f);

	Vector3<float> globalPosition;
	Quaternion globalRotation;
	Vector3<float> globalScale;

	DirectX::XMMATRIX CalculateLocalModelMatrix();

public:
	void UpdateMatrix();
	void UpdateMatrix(DirectX::XMMATRIX localMatrix, bool updateLocalValues = true);

	void SetWorldSpaceMatrix(DirectX::XMMATRIX matrix);
	DirectX::XMMATRIX GetWorldSpaceMatrix();

	void SetPosition(Vector3<float> position);
	void SetRotation(Quaternion quat);

	void SetLocalPosition(Vector3<float> position);
	void SetLocalScale(Vector3<float> scale);
	void SetLocalRotation(Quaternion quat);

	Vector3<float> GetForwardVector();
	Vector3<float> GetUpVector();
	Vector3<float> GetRightVector();

	Vector3<float> GetPosition();
	Vector3<float> GetScale();
	Quaternion GetRotation();

	Vector3<float> GetLocalPosition();
	Vector3<float> GetLocalScale();
	Quaternion GetLocalRotation();

	SceneObject& GetSceneObject();

	// Disable copy constructor and assignment operator
	Transform(const Transform&) = delete;
	Transform& operator=(const Transform&) = delete;

	Transform(SceneObject& sceneObject);

	std::string ToString();
};