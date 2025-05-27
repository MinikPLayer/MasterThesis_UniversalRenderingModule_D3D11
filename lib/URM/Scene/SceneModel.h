#pragma once

#include "SceneObject.h"
#include <URM/Core/ModelLoader.h>
#include <string>

namespace URM::Scene {
	class SceneModel : public SceneObject {
		std::string path;

	public:
		void OnAdded() override;

		SceneModel(std::string path);
	};
}