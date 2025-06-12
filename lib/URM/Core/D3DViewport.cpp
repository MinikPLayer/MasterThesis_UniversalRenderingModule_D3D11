#include "pch.h"
#include "D3DViewport.h"

namespace URM::Core {
	void D3DViewport::Bind(const D3DCore& core) const {
		D3D11_VIEWPORT viewport;
		viewport.Width = static_cast<FLOAT>(this->mData.size.width);
		viewport.Height = static_cast<FLOAT>(this->mData.size.height);
		viewport.MinDepth = this->mData.minDepth;
		viewport.MaxDepth = this->mData.maxDepth;
		viewport.TopLeftX = this->mData.topLeft.x;
		viewport.TopLeftY = this->mData.topLeft.y;

		core.GetContext()->RSSetViewports(1, &viewport);
	}

	void D3DViewport::SetData(const D3DViewportData& newData) {
		this->mData = newData;
	}

	D3DViewport::D3DViewport(const D3DViewportData& newData) : mData(newData) {
		this->SetData(newData);
	}
}
