#pragma once

#include "Mesh.h"
#include "StandardVertexTypes.h"
#include "D3DCore.h"
#include "D3DTexture2D.h"

namespace URM::Core {
	using ModelLoaderVertexType = VertexPositionNormalTexture;

	class ModelLoaderNode {
	public:
		DirectX::XMMATRIX transform;

		std::vector<Mesh<ModelLoaderVertexType>> meshes;
		std::vector<ModelLoaderNode> children;
	};

	class ModelLoader {
		ModelLoader() {}

	public:

		static ModelLoaderNode LoadFromFile(D3DCore& core, std::vector<D3DTexture2D>& loadedTexturesPool, std::string path);
	};
}