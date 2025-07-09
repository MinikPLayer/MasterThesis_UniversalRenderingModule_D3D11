#pragma once

#include "ITest.h"
#include <URM/Engine/ModelObject.h>

class HighPolyModelTest : public ITest {
	bool mUsePBR;

	std::shared_ptr<URM::Engine::SceneObject> mModelsObject;

	void OnInit(URM::Engine::Engine& engine) override;
	void OnUpdate(URM::Engine::Engine& engine) override;

public:
	virtual URM::Core::WindowCreationParams GetWindowParams(HINSTANCE instance) const {
		return URM::Core::WindowCreationParams(1920, 1080, "High poly model", instance);
	}

	HighPolyModelTest(bool usePBR = false) {
		this->mUsePBR = usePBR;
	}
};