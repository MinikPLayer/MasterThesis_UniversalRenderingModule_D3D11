#pragma once

#include <d3d11.h>
#include <vector>
#include <D3DCore.h>

class D3DBuffer {
	ComPtr<ID3D11Buffer> buffer;

	D3DBuffer(D3DCore& core, D3D11_BUFFER_DESC desc, D3D11_SUBRESOURCE_DATA* initData);

public:
	void UpdateWithData(D3DCore& core, const void* data);

	ComPtr<ID3D11Buffer> get() {
		return this->buffer;
	}

	template<typename T>
	static D3DBuffer CreateSingle(D3DCore& core, UINT bindFlags, UINT cpuAccessFlags = 0, D3D11_USAGE usage = D3D11_USAGE_DEFAULT) {
		D3D11_BUFFER_DESC desc = {};
		desc.Usage = usage;
		desc.BindFlags = bindFlags;
		desc.ByteWidth = static_cast<UINT>(sizeof(T));
		desc.CPUAccessFlags = cpuAccessFlags;

		return D3DBuffer(core, desc, nullptr);
	}

	template<typename T>
	static D3DBuffer CreateFromArray(D3DCore& core, std::vector<T> data, UINT bindFlags, UINT cpuAccessFlags = 0, D3D11_USAGE usage = D3D11_USAGE_DEFAULT) {
		D3D11_BUFFER_DESC desc = {};
		desc.Usage = usage;
		desc.BindFlags = bindFlags;
		desc.ByteWidth = static_cast<UINT>(sizeof(T) * data.size());
		desc.CPUAccessFlags = cpuAccessFlags;

		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = data.data();

		return D3DBuffer(core, desc, &initData);
	}
};