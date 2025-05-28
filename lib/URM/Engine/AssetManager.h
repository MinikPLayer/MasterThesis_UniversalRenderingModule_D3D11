#pragma once

#include <URM/Core/D3DTexture2D.h>

namespace URM::Engine {
	class AssetManager : NonCopyable {
	public:
		std::vector<URM::Core::D3DTexture2D> texturePool;
	};
}