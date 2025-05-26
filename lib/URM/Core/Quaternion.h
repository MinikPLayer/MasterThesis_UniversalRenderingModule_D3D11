#pragma once

#include <DirectXMath.h>

#include "Vector2.h"
#include "Vector3.h"

// TODO: Test after GLM -> DXMath conversion.
class Quaternion {
	DirectX::XMVECTOR quat;

public:
	static Quaternion from_euler(Vector3<float> v);

	float X();
	float Y();
	float Z();
	float W();

	[[nodiscard]]
	Vector3<float> forward();

	[[nodiscard]]
	Vector3<float> up();

	[[nodiscard]]
	Vector3<float> right();

	[[nodiscard]]
	Vector3<float> to_euler();

	Vector3<float> RotateVector(Vector3<float> v);

	[[nodiscard]]
	Quaternion rotated_around(Vector3<float> axis, float angle);

	[[nodiscard]]
	Quaternion normalized() const;

	void normalize(float tolerance = 0.0000001f);
	DirectX::XMMATRIX rotation_matrix();

	static Quaternion identity();

	Quaternion() : quat(1, 0, 0, 0) {}
	Quaternion(DirectX::XMVECTOR q) : quat(q) {}
	Quaternion(float v1, float v2, float v3, float v4) : quat(v1, v2, v3, v4) {}
};