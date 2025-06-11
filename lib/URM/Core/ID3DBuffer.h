#pragma once

#include <d3d11.h>
#include "D3DCore.h"

namespace URM::Core {
	class ID3DBuffer {
	protected:
		ComPtr<ID3D11Buffer> mBuffer;

		ID3DBuffer(const D3DCore& core, const D3D11_BUFFER_DESC& desc, const D3D11_SUBRESOURCE_DATA* initData);
	public:
		virtual ~ID3DBuffer() = default;
		virtual void Bind(D3DCore& core, UINT slot) = 0;
		void UpdateWithData(const D3DCore& core, const void* data) const;

		ComPtr<ID3D11Buffer> Get() {
			return this->mBuffer;
		}
	};
}
