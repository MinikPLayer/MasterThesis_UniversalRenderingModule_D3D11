#include "pch.h"
#include "D3DViewport.h"

namespace URM::Core {
    void D3DViewport::Bind(D3DCore& core) {
        D3D11_VIEWPORT viewport;
        viewport.Width = static_cast<FLOAT>(this->data.size.width);
        viewport.Height = static_cast<FLOAT>(this->data.size.height);
        viewport.MinDepth = this->data.minDepth;
        viewport.MaxDepth = this->data.maxDepth;
        viewport.TopLeftX = this->data.topLeft.x;
        viewport.TopLeftY = this->data.topLeft.y;

        core.GetContext()->RSSetViewports(1, &viewport);
    }

    void D3DViewport::SetData(D3DViewportData newData) {
        this->data = newData;
    }

    D3DViewport::D3DViewport(D3DViewportData newData) : data(newData) {
        this->SetData(newData);
    }
}