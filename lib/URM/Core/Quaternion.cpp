#include "pch.h"
#include "Quaternion.h"

Quaternion Quaternion::from_euler(Vector3<float> v) {
	return Quaternion(
		DirectX::XMQuaternionRotationRollPitchYaw(
			DirectX::XMConvertToRadians(v.x),
			DirectX::XMConvertToRadians(v.y),
			DirectX::XMConvertToRadians(v.z)
		)
	);
}

float Quaternion::X() {
	return DirectX::XMVectorGetX(quat);
}

float Quaternion::Y() {
	return DirectX::XMVectorGetY(quat);
}

float Quaternion::Z() {
	return DirectX::XMVectorGetZ(quat);
}

float Quaternion::W() {
	return DirectX::XMVectorGetW(quat);
}

[[nodiscard]]
Vector3<float> Quaternion::forward() {
	return RotateVector(Vector3<float>(0, 0, 1));
}

[[nodiscard]]
Vector3<float> Quaternion::up() {
	return RotateVector(Vector3<float>(0, 1, 0));
}

[[nodiscard]]
Vector3<float> Quaternion::right() {
	return RotateVector(Vector3<float>(1, 0, 0));
}

// Source: https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
[[nodiscard]]
Vector3<float> Quaternion::to_euler() {
	auto x = X();
	auto y = Y();
	auto z = Z();
	auto w = W();

	auto sine_x_cosine_y = 2 * (w * x + y * z);
	auto cosine_x_cosine_y = 1 - 2 * (x * x + y * y);
	auto dx = DirectX::XMConvertToDegrees(atan2(sine_x_cosine_y, cosine_x_cosine_y));

	auto sine_y = sqrt(1 + 2 * (w * y - x * z));
	auto cosine_y = sqrt(1 - 2 * (w * y - x * z));
	auto dy = DirectX::XMConvertToDegrees(2 * atan2(sine_y, cosine_y) - DirectX::XM_PI / 2);

	auto sine_z_cosine_y = 2 * (w * z + x * y);
	auto cosine_z_cosine_y = 1 - 2 * (y * y + z * z);
	auto dz = DirectX::XMConvertToDegrees(atan2(sine_z_cosine_y, cosine_z_cosine_y));

	return Vector3<float>(dx, dy, dz);
}

Vector3<float> Quaternion::RotateVector(Vector3<float> v) {
	auto rotated = DirectX::XMVector3Rotate(
		v.ToXMVector(),
		quat
	);

	auto xmFloat3 = DirectX::XMFLOAT3();
	DirectX::XMStoreFloat3(&xmFloat3, rotated);
	return Vector3<float>(xmFloat3);
}

[[nodiscard]]
Quaternion Quaternion::rotated_around(Vector3<float> axis, float angle) {
	auto rotated = DirectX::XMQuaternionRotationAxis(
		axis.ToXMVector(),
		angle
	);

	return Quaternion(
		DirectX::XMQuaternionMultiply(rotated, quat)
	);
}

[[nodiscard]]
Quaternion Quaternion::normalized() const {
	auto ret = Quaternion(*this);
	ret.normalize();
	return ret;
}

void Quaternion::normalize(float tolerance) {
	auto x = X();
	auto y = Y();
	auto z = Z();
	auto w = W();

	float mag2 = x * x + y * y + z * z + w * w;
	if (mag2 - 1.0f > tolerance) {
		float mag = sqrt(mag2);
		x /= mag;
		y /= mag;
		z /= mag;
		w /= mag;
	}
}

DirectX::XMMATRIX Quaternion::rotation_matrix() {
	return DirectX::XMMatrixRotationQuaternion(quat);
}

Quaternion Quaternion::identity() {
	return Quaternion();
}