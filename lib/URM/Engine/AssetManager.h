#pragma once

#include <URM/Core/D3DTexture2D.h>

#include "URM/Core/Mesh.h"
#include "URM/Core/ModelLoader.h"

namespace URM::Engine {
	class Scene;
	class ModelAsset {
	public:
		std::string path;
		std::shared_ptr<Core::ModelLoaderNode> rootNode;

		ModelAsset(const std::string& path, const std::shared_ptr<Core::ModelLoaderNode>& rootNode);
	};
	
	class AssetManager final : NonCopyable {
		std::reference_wrapper<Scene> mScene;
		std::vector<ModelAsset> mModelPool;

		void AddModelToCache(const std::string& path, const std::shared_ptr<Core::ModelLoaderNode>& rootNode, bool overrideIfExists = false);
		std::optional<std::shared_ptr<Core::ModelLoaderNode>> GetCachedModel(const std::string& path);
		bool IsModelCached(const std::string& path) const;
	public:
		std::vector<Core::D3DTexture2D> texturePool;

		bool RemoveCachedModel(const std::string& path);
		std::shared_ptr<Core::ModelLoaderNode> GetModel(const std::string& path);
		
		void ClearAll();

		AssetManager(Scene& scene);
	};
}
