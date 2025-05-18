#pragma once
#include <d3d11.h>
#include <concepts>
#include <vector>

#include "D3DVertexBuffer.h"
#include "D3DIndexBuffer.h"
#include "VertexConcept.h"
#include "IMesh.h"

template<VertexTypeConcept VertexType>
class Mesh : public IMesh {
	D3DVertexBuffer<VertexType> vertexBuffer;
	std::optional<D3DIndexBuffer> indexBuffer;

	std::vector<VertexType> vertices;
	std::vector<unsigned int> indices;

	// TODO: Test
	void UpdateBuffer(D3DCore& core);

	size_t GetVertexTypeHashCode() override {
		return GetTypeHashCode<VertexType>();
	}
public:
	D3DVertexBuffer<VertexType>& GetVertexBuffer() {
		return vertexBuffer;
	}

	// TODO: Add index buffer support
	Mesh(D3DCore& core, std::vector<VertexType> data)
		: vertexBuffer(D3DVertexBuffer<VertexType>::Create(core, data)) 
	{
		this->vertices = data;
	}
	
	void ResetVertices(D3DCore& core) override {
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
		vertexBuffer.get().Get(),
		0,
		nullptr,
		vertices.data(),
		(UINT)(sizeof(VertexType) * vertices.size()),
		sizeof(VertexType)
	);
}
