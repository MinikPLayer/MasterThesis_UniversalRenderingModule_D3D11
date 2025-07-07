#pragma once

#include "ITest.h"

class MaterialTest : public ITest {
	std::shared_ptr<URM::Engine::LightObject> mLight;
	std::shared_ptr<URM::Engine::SceneObject> mModelsObject;
	void OnInit(URM::Engine::Engine& engine) override;
	void OnUpdate(URM::Engine::Engine& engine) override;
};