#pragma once
#include <d3d11.h>
#include <concepts>
#include <vector>

#include "D3DVertexBuffer.h"
#include "D3DIndexBuffer.h"
#include "VertexConcept.h"
#include "IMesh.h"
#include "MaterialProperty.h"
#include "D3DTexture2D.h"

template<VertexTypeConcept VertexType>
class Mesh : public IMesh {
	D3DVertexBuffer<VertexType> vertexBuffer;
	std::optional<D3DIndexBuffer> indexBuffer = std::nullopt;

	std::vector<VertexType> vertices;
	std::vector<unsigned int> indices;
	std::vector<D3DTexture2D> textures;

	// TODO: Test
	void UpdateBuffer(D3DCore& core);

	size_t GetVertexTypeHashCode() override {
		return GetTypeHashCode<VertexType>();
	}
public:
	std::vector<MaterialProperty> materialProperties;

	bool ContainsTextures() {
		return !textures.empty();
	}

	bool ContainsIndices() const {
		return indexBuffer.has_value();
	}

	D3DVertexBuffer<VertexType>& GetVertexBuffer() {
		return vertexBuffer;
	}

	D3DIndexBuffer& GetIndexBuffer() {
		if (indexBuffer.has_value()) {
			return indexBuffer.value();
		}
		throw std::runtime_error("Index buffer not available");
	}

	UINT GetVerticesCount() {
		return (UINT)vertices.size();
	}

	UINT GetIndicesCount() {
		if (indexBuffer.has_value()) {
			return (UINT)indices.size();
		}
		return 0;
	}

	// TODO: Add index buffer support
	Mesh(D3DCore& core, std::vector<VertexType> data, std::vector<D3DTexture2D> textures = {})
		: vertexBuffer(D3DVertexBuffer<VertexType>::Create(core, data)) 
	{
		this->vertices = data;
		this->textures = textures;
	}

	Mesh(D3DCore& core, std::vector<VertexType> data, std::vector<unsigned int> indices, std::vector<D3DTexture2D> textures = {})
		: vertexBuffer(D3DVertexBuffer<VertexType>::Create(core, data)),
		indexBuffer(D3DIndexBuffer::Create(core, indices))
	{
		this->vertices = data;
		this->indices = indices;
		this->textures = textures;
	}
	
	void ResetVertices(D3DCore& core) override {
		vertices.clear();
		UpdateBuffer(core);
	}

	void SetVertices(D3DCore& core, const std::vector<VertexType>& newVertices) {
		vertices = newVertices;
		UpdateBuffer(core);
	}

	void ResetIndices(D3DCore& core) {
		indices.clear();
		if (indexBuffer.has_value()) {
			indexBuffer->UpdateWithData(core, indices.data());
		}
	}

	void SetIndices(D3DCore& core, const std::vector<unsigned int>& newIndices) {
		indices = newIndices;
		if (indexBuffer.has_value()) {
			indexBuffer->UpdateWithData(core, indices.data());
		}
	}

	const std::vector<VertexType> GetVerticesCopy() const {
		return vertices;
	}

	const std::vector<unsigned int> GetIndicesCopy() const {
		return indices;
	}

	std::vector<D3DTexture2D> GetTexturesCopy() const {
		return textures;
	}

	void BindTextures(D3DCore& core) {
		for (UINT i = 0; i < (UINT)textures.size(); i++) {
			textures[i].Bind(core, i);
		}
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
