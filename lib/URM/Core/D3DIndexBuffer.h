#pragma once

#include "VertexConcept.h"
#include "D3DCore.h"
#include "ID3DBuffer.h"

namespace URM::Core {
	class D3DIndexBuffer : public ID3DBuffer {
		using IndexBufferType = unsigned int;

		D3DIndexBuffer(D3DCore& core, D3D11_BUFFER_DESC desc, D3D11_SUBRESOURCE_DATA* initData)
			: ID3DBuffer(core, desc, initData) {
		}

	public:
		void Bind(D3DCore& core, UINT offset) override {
			core.GetContext()->IASetIndexBuffer(this->buffer.Get(), DXGI_FORMAT_R32_UINT, offset);
		}

		static D3DIndexBuffer Create(D3DCore& core, std::vector<IndexBufferType> data, UINT cpuAccessFlags = 0, D3D11_USAGE usage = D3D11_USAGE_DEFAULT) {
			D3D11_BUFFER_DESC desc = {};
			desc.Usage = usage;
			desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			desc.ByteWidth = static_cast<UINT>(sizeof(IndexBufferType) * data.size());
			desc.CPUAccessFlags = cpuAccessFlags;

			D3D11_SUBRESOURCE_DATA initData = {};
			initData.pSysMem = data.data();

			return D3DIndexBuffer(core, desc, &initData);
		}
	};
}