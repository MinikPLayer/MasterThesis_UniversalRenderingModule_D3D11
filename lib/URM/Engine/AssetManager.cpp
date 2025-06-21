#include "pch.h"
#include "AssetManager.h"

#include "Scene.h"

namespace URM::Engine {
	ModelAsset::ModelAsset(const std::string& path, const std::shared_ptr<Core::ModelLoaderNode>& rootNode) {
		this->path = path;
		this->rootNode = rootNode;
	}
	
	void AssetManager::AddModelToCache(const std::string& path, const std::shared_ptr<Core::ModelLoaderNode>& rootNode, const bool overrideIfExists) {
		if (!overrideIfExists && IsModelCached(path)) {
			throw std::runtime_error("Cannot add model that is already loaded: " + path);
		}

		this->mModelPool.push_back(ModelAsset(path, rootNode));
	}
	
	bool AssetManager::RemoveCachedModel(const std::string& path) {
		for (auto it = this->mModelPool.begin(); it != this->mModelPool.end(); it++) {
			if (it->path == path) {
				this->mModelPool.erase(it);
				return true;
			}
		}

		return false;
	}
	
	std::shared_ptr<Core::ModelLoaderNode> AssetManager::GetModel(const std::string& path) {
		if (auto cachedModel = this->GetCachedModel(path); cachedModel.has_value()) {
			spdlog::trace("Loading cached model: {}", path);
			return cachedModel.value();
		}

		spdlog::trace("Loading new model: {}", path);
		const auto model = Core::ModelLoader::LoadFromFile(this->mScene.get().GetCore(), this->texturePool, path);
		AddModelToCache(path, model);

		return model;
	}

	bool AssetManager::IsModelCached(const std::string& path) const {
		for (auto& m : this->mModelPool) {
			if (m.path == path) {
				return true;
			}
		}

		return false;
	}

	std::optional<std::shared_ptr<Core::ModelLoaderNode>> AssetManager::GetCachedModel(const std::string& path) {
		for (auto& m : this->mModelPool) {
			if (m.path == path) {
				return m.rootNode;
			}
		}

		return std::nullopt;
	}
	
	void AssetManager::ClearAll() {
		this->mModelPool.clear();
		this->texturePool.clear();
	}
	
	AssetManager::AssetManager(Scene& scene) : mScene(scene) {}
}
