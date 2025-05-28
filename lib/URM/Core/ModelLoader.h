#pragma once

#include "Mesh.h"
#include "StandardVertexTypes.h"
#include "D3DCore.h"
#include "D3DTexture2D.h"

namespace URM::Core {
	using ModelLoaderVertexType = VertexPositionNormalTexture;

	class ModelLoaderNode : NonCopyable {
	public:
		DirectX::XMMATRIX transform;

		std::vector<std::shared_ptr<Mesh<ModelLoaderVertexType>>> meshes;
		std::vector<std::shared_ptr<ModelLoaderNode>> children;
	};

	class ModelLoader {
		ModelLoader() {}
	public:
		static std::shared_ptr<ModelLoaderNode> LoadFromFile(D3DCore& core, std::vector<D3DTexture2D>& loadedTexturesPool, const std::string& path);
	};
}
