#pragma once

#include "pch.h"
#include "D3DCore.h"
#include "VertexConcept.h"

//template<VertexTypeConcept VertexType>
//class Mesh;

class IMesh {
protected:
	IMesh() {}

	template<VertexTypeConcept VertexType>
	static size_t GetTypeHashCode() {
		return typeid(VertexType).hash_code();
	}

	virtual void UpdateBuffer(D3DCore& core) = 0;
	virtual size_t GetVertexTypeHashCode() = 0;
public:

	virtual void ResetVertices(D3DCore& core) = 0;

	template<VertexTypeConcept VertexType>
	void GetImplementation() { 
		auto templateHashCode = GetTypeHashCode<VertexType>();
		auto implementationHashCode = GetVertexTypeHashCode();

		if (templateHashCode != implementationHashCode) {
			throw std::runtime_error("Invalid vertex type");
		}
	}
};