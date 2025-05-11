#pragma once
#include <d3d11.h>
#include <concepts>
#include <vector>

template<typename T>
concept VertexTypeConcept = requires {
	{ T::GetInputLayout() } -> std::same_as<std::vector<D3D11_INPUT_ELEMENT_DESC>>;
};

template<VertexTypeConcept VertexType>
class Mesh {
	std::vector<VertexType> vertices;

public:
	Mesh() = default;
	void RemoveVertexAt(size_t index) {
		if (index < vertices.size()) {
			vertices.erase(vertices.begin() + index);
		}
	}
	
	void ResetVertices() {
		vertices.clear();
	}

	void SetVertices(const std::vector<VertexType>& newVertices) {
		vertices = newVertices;
	}

	void InsertVertex(const std::vector<VertexType>& newVertices) {
		vertices.insert(vertices.end(), newVertices.begin(), newVertices.end());
	}

	void AddVertices(const std::initializer_list<VertexType> newVertices) {
		vertices.insert(vertices.end(), newVertices.begin(), newVertices.end());
	}

	void AddVertex(const VertexType& vertex) {
		vertices.push_back(vertex);
	}

	const std::vector<VertexType>& GetVertices() const {
		return vertices;
	}
};