#pragma once
#include <d3d11.h>
#include <vector>

#include "D3DVertexBuffer.h"
#include "D3DIndexBuffer.h"
#include "VertexConcept.h"
#include "IMesh.h"
#include "MaterialProperty.h"
#include "D3DTexture2D.h"

namespace URM::Core {
	template<VertexTypeConcept VertexType>
	class Mesh : public IMesh, NonCopyable {
		D3DVertexBuffer<VertexType> mVertexBuffer;
		std::optional<D3DIndexBuffer> mIndexBuffer = std::nullopt;

		std::vector<VertexType> mVertices;
		std::vector<unsigned int> mIndices;
		std::vector<D3DTexture2D> mTextures;

		// [TEST]
		void UpdateBuffer(D3DCore& core) override;

		size_t GetVertexTypeHashCode() override {
			return GetTypeHashCode<VertexType>();
		}
	public:
		std::vector<MaterialProperty> materialProperties;

		bool ContainsTextures() const {
			return !mTextures.empty();
		}

		bool ContainsIndices() const {
			return mIndexBuffer.has_value();
		}

		D3DVertexBuffer<VertexType>& GetVertexBuffer() {
			return mVertexBuffer;
		}

		D3DIndexBuffer& GetIndexBuffer() {
			if (mIndexBuffer.has_value()) {
				return mIndexBuffer.value();
			}
			throw std::runtime_error("Index buffer not available");
		}

		UINT GetVerticesCount() {
			return static_cast<UINT>(mVertices.size());
		}

		UINT GetIndicesCount() const {
			if (mIndexBuffer.has_value()) {
				return static_cast<UINT>(mIndices.size());
			}
			return 0;
		}

		Mesh(D3DCore& core, std::vector<VertexType> data, std::vector<D3DTexture2D> textures = {}) : mVertexBuffer(D3DVertexBuffer<VertexType>::Create(core, data)) {
			this->mVertices = data;
			this->mTextures = textures;
		}

		Mesh(D3DCore& core, std::vector<VertexType> data, std::vector<unsigned int> indices, std::vector<D3DTexture2D> textures = {}) : mVertexBuffer(D3DVertexBuffer<VertexType>::Create(core, data)),
		                                                                                                                                mIndexBuffer(D3DIndexBuffer::Create(core, indices)) {
			this->mVertices = data;
			this->mIndices = indices;
			this->mTextures = textures;
		}

		void ResetVertices(D3DCore& core) override {
			mVertices.clear();
			UpdateBuffer(core);
		}

		void SetVertices(D3DCore& core, const std::vector<VertexType>& newVertices) {
			mVertices = newVertices;
			UpdateBuffer(core);
		}

		void ResetIndices(const D3DCore& core) {
			mIndices.clear();
			if (mIndexBuffer.has_value()) {
				mIndexBuffer->UpdateWithData(core, mIndices.data());
			}
		}

		void SetIndices(const D3DCore& core, const std::vector<unsigned int>& newIndices) {
			mIndices = newIndices;
			if (mIndexBuffer.has_value()) {
				mIndexBuffer->UpdateWithData(core, mIndices.data());
			}
		}

		std::vector<VertexType> GetVerticesCopy() const {
			return mVertices;
		}

		std::vector<unsigned int> GetIndicesCopy() const {
			return mIndices;
		}

		std::vector<D3DTexture2D> GetTexturesCopy() const {
			return mTextures;
		}

		void BindTextures(const D3DCore& core) {
			for (UINT i = 0; i < static_cast<UINT>(mTextures.size()); i++) {
				mTextures[i].Bind(core, i);
			}
		}
	};

	template<VertexTypeConcept VertexType>
	void Mesh<VertexType>::UpdateBuffer(D3DCore& core) {
		core.GetContext()->UpdateSubresource(
			mVertexBuffer.Get().Get(),
			0,
			nullptr,
			mVertices.data(),
			static_cast<UINT>(sizeof(VertexType) * mVertices.size()),
			sizeof(VertexType)
		);
	}
}
