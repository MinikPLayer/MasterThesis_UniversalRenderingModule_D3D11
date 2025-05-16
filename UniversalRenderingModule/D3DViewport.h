#pragma once

#include "D3DCore.h"
#include <d3d11.h>

struct D3DViewportData {
	Size2i size;
	float minDepth;
	float maxDepth;
	Pos2i topLeft;

	D3DViewportData(Size2i size, float minDepth = 0.0f, float maxDepth = 1.0f, Pos2i topLeft = Pos2i::ZERO)
		: size(size), minDepth(minDepth), maxDepth(maxDepth), topLeft(topLeft) {
	}
};

class D3DViewport {
	D3D11_VIEWPORT viewport;
	D3DViewportData data;

public:
	void Bind(D3DCore& core);

	D3DViewportData GetData() {
		return this->data;
	}
	void Set(D3DViewportData data);

	D3DViewport(D3DViewportData data);
};