#include "pch.h"
#include "D3DViewport.h"

void D3DViewport::Bind(D3DCore& core) {
	core.GetContext()->RSSetViewports(1, &this->viewport);
}

void D3DViewport::Set(D3DViewportData data) {
    this->data = data;

    this->viewport.Width = static_cast<FLOAT>(data.size.width);
    this->viewport.Height = static_cast<FLOAT>(data.size.height);
    this->viewport.MinDepth = data.minDepth;
    this->viewport.MaxDepth = data.maxDepth;
    this->viewport.TopLeftX = data.topLeft.x;
    this->viewport.TopLeftY = data.topLeft.y;
}

D3DViewport::D3DViewport(D3DViewportData data) : data(data) {
    this->Set(data);
}
