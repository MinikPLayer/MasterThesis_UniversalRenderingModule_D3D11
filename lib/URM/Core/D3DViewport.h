#pragma once

#include "D3DCore.h"
#include <d3d11.h>

struct D3DViewportData {
	Vector2i size;
	float minDepth;
	float maxDepth;
	Vector2f topLeft;

	D3DViewportData(Vector2i size, float minDepth = 0.0f, float maxDepth = 1.0f, Vector2f topLeft = Vector2f(0, 0))
		: size(size), minDepth(minDepth), maxDepth(maxDepth), topLeft(topLeft) { 
	}
};

class D3DViewport {
	D3DViewportData data;

public:
	void Bind(D3DCore& core);

	D3DViewportData GetData() {
		return this->data;
	}
	void SetData(D3DViewportData data);

	D3DViewport(D3DViewportData data);
};