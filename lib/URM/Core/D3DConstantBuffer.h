#pragma once

#include "ID3DBuffer.h"

class D3DConstantBuffer : public ID3DBuffer {
	D3DConstantBuffer(D3DCore& core, D3D11_BUFFER_DESC desc, D3D11_SUBRESOURCE_DATA* initData)
		: ID3DBuffer(core, desc, initData) {
	}

public:

	void Bind(D3DCore& core) override {
		core.GetContext()->VSSetConstantBuffers(0, 1, this->buffer.GetAddressOf());
	}

	template<typename T>
	static D3DConstantBuffer Create(D3DCore& core, UINT cpuAccessFlags = 0, D3D11_USAGE usage = D3D11_USAGE_DEFAULT) {
		D3D11_BUFFER_DESC desc = {};
		desc.Usage = usage;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.ByteWidth = static_cast<UINT>(sizeof(T));
		desc.CPUAccessFlags = cpuAccessFlags;

		return D3DConstantBuffer(core, desc, nullptr);
	}
};