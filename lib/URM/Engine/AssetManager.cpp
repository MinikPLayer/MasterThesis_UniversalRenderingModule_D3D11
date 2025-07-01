#include "pch.h"
#include "AssetManager.h"

#include "Scene.h"

namespace URM::Engine {
	void AssetManager::AddModelToCache(const std::string& path, const std::shared_ptr<Core::ModelLoaderNode>& rootNode, const bool overrideIfExists) {
		if (!overrideIfExists && IsModelCached(path)) {
			throw std::runtime_error("Cannot add model that is already loaded: " + path);
		}

		this->mModelPool[path] = rootNode;
	}
	
	bool AssetManager::RemoveCachedModel(const std::string& path) {
		return this->mModelPool.erase(path) > 0;
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
		return this->mModelPool.count(path) > 0;
	}

	std::optional<std::shared_ptr<Core::ModelLoaderNode>> AssetManager::GetCachedModel(const std::string& path) {
		try {
			return this->mModelPool.at(path);
		}
		catch(std::out_of_range&) {
			return std::nullopt;
		}
	}
	
	void AssetManager::ClearAll() {
		this->mModelPool.clear();
		this->texturePool.clear();
	}
	
	AssetManager::AssetManager(Scene& scene) : mScene(scene) {}
}
