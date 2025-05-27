#pragma once

#include <d3d11.h>
#include <vector>
#include "D3DCore.h"

namespace URM::Core {
	class ID3DBuffer {
	protected:

		ComPtr<ID3D11Buffer> buffer;

		ID3DBuffer(D3DCore& core, D3D11_BUFFER_DESC desc, D3D11_SUBRESOURCE_DATA* initData);

	public:
		virtual void Bind(D3DCore& core, UINT slot) = 0;
		void UpdateWithData(D3DCore& core, const void* data);

		ComPtr<ID3D11Buffer> get() {
			return this->buffer;
		}
	};
}