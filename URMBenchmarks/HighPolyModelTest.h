#pragma once

#include "ITest.h"
#include <URM/Engine/ModelObject.h>

class HighPolyModelTest : public ITest {
	std::shared_ptr<URM::Engine::SceneObject> mModelsObject;

	void OnInit(URM::Engine::Engine& engine) override;
	void OnUpdate(URM::Engine::Engine& engine) override;
};