#pragma once

#include <URM/Core/D3DTexture2D.h>

namespace URM::Scene {
	class AssetManager {
	public:
		std::vector<URM::Core::D3DTexture2D> texturePool;
	};
}