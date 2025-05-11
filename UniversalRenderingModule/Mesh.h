#pragma once
#include <d3d11.h>
#include <concepts>
#include <vector>

#include "D3DBuffer.h"
#include "VertexConcept.h"

template<VertexTypeConcept VertexType>
class Mesh {
	D3DBuffer vertexBuffer;
	std::vector<VertexType> vertices;

	// TODO: Test
	void UpdateBuffer(D3DCore& core);

public:
	D3DBuffer& GetVertexBuffer() {
		return vertexBuffer;
	}

	Mesh(D3DCore& core, std::vector<VertexType> data)
		: vertexBuffer(D3DBuffer::CreateFromArray(core, data, D3D11_BIND_VERTEX_BUFFER)) {}
	
	void ResetVertices(D3DCore& core) {
		vertices.clear();
		UpdateBuffer(core);
	}

	void SetVertices(D3DCore& core, const std::vector<VertexType>& newVertices) {
		vertices = newVertices;
		UpdateBuffer(core);
	}

	const std::vector<VertexType> GetVerticesCopy() const {
		return vertices;
	}
};

template<VertexTypeConcept VertexType>
inline void Mesh<VertexType>::UpdateBuffer(D3DCore& core) {
	core.GetContext()->UpdateSubresource(
		vertexBuffer.get(),
		0,
		nullptr,
		vertices.data(),
		sizeof(VertexType) * vertices.size(),
		sizeof(VertexType)
	);
}
