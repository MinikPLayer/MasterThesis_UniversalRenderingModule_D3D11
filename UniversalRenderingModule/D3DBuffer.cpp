#include "pch.h"
#include "D3DBuffer.h"

static void ReleaseBuffer(ID3D11Buffer* buffer) {
	if (buffer) {
		buffer->Release();
	}
}

D3DBuffer::D3DBuffer(D3DCore& core, D3D11_BUFFER_DESC desc, D3D11_SUBRESOURCE_DATA* initData)
{
	core.GetDevice()->CreateBuffer(&desc, initData, this->buffer.GetAddressOf());
}

void D3DBuffer::UpdateWithData(D3DCore& core, const void* data) {
	auto context = core.GetContext();
	context->UpdateSubresource(
		this->buffer.Get(),
		0,
		nullptr,
		data,
		0,
		0
	);
}
