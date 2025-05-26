#pragma once

#include <math.h>
#include "Utils.h"
#include <string>
#include <sstream>

template<typename T>
class Vector2 {
public:
	T x, y;

	static Vector2<T> zero() {
		return Vector2<T>(0, 0);
	}

	static Vector2<T> one() {
		return Vector2<T>(1, 1);
	}

	Vector2(T x = 0, T y = 0) {
		this->x = x;
		this->y = y;
	}

	bool is_equal_exact(Vector2<T> const& v) const {
		return x == v.x && y == v.y;
	}

	bool is_equal_approximate(Vector2<T> const& v, float epsilon = 0.00001f) const {
		return FloatUtils::IsEqualApproximate(x, v.x, epsilon) &&
			FloatUtils::IsEqualApproximate(y, v.y, epsilon);
	}

	Vector2<T> operator+(Vector2<T> const& v) {
		return Vector2<T>(v.x + x, v.y + y);
	}
	Vector2<T> operator-(Vector2<T> const& v) {
		return Vector2<T>(x - v.x, y - v.y);
	}
	Vector2<T> operator*(float const& f) {
		return Vector2<T>((T)(x * f), (T)(y * f));
	}
	Vector2<T> operator/(float const& f) {
		return Vector2<T>((T)(x / f), (T)(y / f));
	}

	float dot(Vector2<T> const& v) {
		return v.x * x + v.y * y;
	}

	T sqr_magnitude() {
		return x * x + y * y;
	}

	double magnitude() {
		return sqrt(sqr_magnitude());
	}

	Vector2<T> normalized() {
		return *this / magnitude();
	}

	std::string to_string() const {
		std::stringstream ss;
		ss << "(" << x << ", " << y << ")";
		return ss.str();
	}
};

using Vector2f = Vector2<float>;
using Vector2i = Vector2<int>;