#pragma once

#include <DirectXMath.h>

namespace URM::Core {
	struct Color {
		float r, g, b, a;

		DirectX::XMVECTORF32 ToXM() {
			return {{{ r, g, b, a }}};
		}

		Color() {
			this->r = 0;
			this->g = 0;
			this->b = 0;
			this->a = 1;
		}

		Color(float r, float g, float b, float a = 1.0f) {
			this->r = r;
			this->g = g;
			this->b = b;
			this->a = a;
		}
	};
}