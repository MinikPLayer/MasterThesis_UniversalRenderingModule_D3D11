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