#pragma once

#include "ID3DBuffer.h"
#include "VertexConcept.h"

namespace URM::Core {
	template<VertexTypeConcept VertexType>
	class D3DVertexBuffer : public ID3DBuffer {
		D3DVertexBuffer(D3DCore& core, D3D11_BUFFER_DESC desc, D3D11_SUBRESOURCE_DATA* initData)
			: ID3DBuffer(core, desc, initData) {
		}

	public:
		void Bind(D3DCore& core, UINT slot) override {
			UINT stride = sizeof(VertexType);
			UINT offset = 0;

			core.GetContext()->IASetVertexBuffers(slot, 1, this->buffer.GetAddressOf(), &stride, &offset);
		}

		static D3DVertexBuffer Create(D3DCore& core, std::vector<VertexType> data, UINT cpuAccessFlags = 0, D3D11_USAGE usage = D3D11_USAGE_DEFAULT) {
			D3D11_BUFFER_DESC desc = {};
			desc.Usage = usage;
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			desc.ByteWidth = static_cast<UINT>(sizeof(VertexType) * data.size());
			desc.CPUAccessFlags = cpuAccessFlags;

			D3D11_SUBRESOURCE_DATA initData = {};
			initData.pSysMem = data.data();

			return D3DVertexBuffer(core, desc, &initData);
		}
	};
}