#include "pch.h"
#include "iD3DBuffer.h"

namespace URM::Core {
	ID3DBuffer::ID3DBuffer(const D3DCore& core, const D3D11_BUFFER_DESC& desc, const D3D11_SUBRESOURCE_DATA* initData) {
		core.GetDevice()->CreateBuffer(&desc, initData, this->mBuffer.GetAddressOf());
	}

	void ID3DBuffer::UpdateWithData(const D3DCore& core, const void* data) const {
		auto context = core.GetContext();
		context->UpdateSubresource(
			this->mBuffer.Get(),
			0,
			nullptr,
			data,
			0,
			0
		);
	}
}
