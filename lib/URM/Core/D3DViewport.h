#pragma once

#include "D3DCore.h"

namespace URM::Core {
	struct D3DViewportData {
		Size2i size;
		float minDepth;
		float maxDepth;
		Vector2 topLeft;

		D3DViewportData(Size2i size = Size2i::ZERO, float minDepth = 0.0f, float maxDepth = 1.0f, Vector2 topLeft = Vector2::Zero) : size(size), minDepth(minDepth), maxDepth(maxDepth), topLeft(topLeft) {}
	};

	class D3DViewport {
		D3DViewportData mData;
	public:
		void Bind(const D3DCore& core) const;

		D3DViewportData GetData() const {
			return this->mData;
		}
		void SetData(const D3DViewportData& data);

		D3DViewport(const D3DViewportData& data = D3DViewportData());
	};
}
