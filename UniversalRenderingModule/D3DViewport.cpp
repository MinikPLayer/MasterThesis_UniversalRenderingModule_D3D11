#include "pch.h"
#include "D3DViewport.h"

void D3DViewport::Bind(D3DCore& core) {
	core.GetContext()->RSSetViewports(1, &this->viewport);
}

void D3DViewport::Set(D3DViewportData newData) {
    this->data = newData;

    this->viewport.Width = static_cast<FLOAT>(newData.size.width);
    this->viewport.Height = static_cast<FLOAT>(newData.size.height);
    this->viewport.MinDepth = newData.minDepth;
    this->viewport.MaxDepth = newData.maxDepth;
    this->viewport.TopLeftX = newData.topLeft.x;
    this->viewport.TopLeftY = newData.topLeft.y;
}

D3DViewport::D3DViewport(D3DViewportData newData) : data(newData) {
    this->Set(newData);
}
