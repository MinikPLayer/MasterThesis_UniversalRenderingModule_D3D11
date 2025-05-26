#pragma once

#include <math.h>
#include "Utils.h"
#include <string>
#include <sstream>
#include <DirectXMath.h>

template<typename T>
class Vector3 {
public:
	T x, y, z;

	Vector3(T x = 0, T y = 0, T z = 0) {
		this->x = x;
		this->y = y;
		this->z = z;
	}

	Vector3(DirectX::XMFLOAT3 v) {
		x = v.x;
		y = v.y;
		z = v.z;
	}

	bool IsEqualExact(Vector3<T> const& v) const {
		return x == v.x && y == v.y && z == v.z;
	}

	bool IsEqualApproximate(Vector3<T> const& v, float epsilon = 0.00001f) const {
		return FloatUtils::IsEqualApproximate((float)x, (float)v.x, epsilon) &&
			FloatUtils::IsEqualApproximate((float)y, (float)v.y, epsilon) &&
			FloatUtils::IsEqualApproximate((float)z, (float)v.z, epsilon);
	}

	DirectX::XMFLOAT3 ToXMFloat3() {
		return DirectX::XMFLOAT3(x, y, z);
	}

	DirectX::XMVECTOR ToXMVector() {
		auto xfloat3 = ToXMFloat3();
		return DirectX::XMLoadFloat3(&xfloat3);
	}

	Vector3<T> operator+(Vector3<T> const& v) {
		return Vector3<T>(v.x + x, v.y + y, v.z + z);
	}
	Vector3<T> operator-(Vector3<T> const& v) {
		return Vector3<T>(x - v.x, y - v.y, z - v.z);
	}
	Vector3<T> operator*(float const& f) {
		return Vector3<T>(x * f, y * f, z * f);
	}
	Vector3<T> operator/(float const& f) {
		return Vector3<T>(x / f, y / f, z / f);
	}

	friend Vector3<T> operator*(float const& f, Vector3<T> const& v) {
		return Vector3<T>(v.x * f, v.y * f, v.z * f);
	}

	friend Vector3<T> operator+(float const& f, Vector3<T> const& v) {
		return Vector3<T>(f + v.x, f + v.y, f + v.z);
	}

	float Dot(Vector3<T> const& v) {
		return (float)(v.x * x + v.y * y + v.z * z);
	}

	Vector3<T> Cross(Vector3<T> const& v) {
		return Vector3<T>(
			y * v.z - z * v.y,
			z * v.x - x * v.z,
			x * v.y - y * v.x
		);
	}

	T SqrMagnitude() {
		return x * x + y * y + z * z;
	}

	double Magnitude() {
		return sqrt(SqrMagnitude());
	}

	Vector3<T> Normalized() {
		return *this / Magnitude();
	}

	std::string ToString() const {
		std::stringstream ss;
		ss << "(" << x << ", " << y << ", " << z << ")";
		return ss.str();
	}
};

using Vector3f = Vector3<float>;
using Vector3i = Vector3<int>;