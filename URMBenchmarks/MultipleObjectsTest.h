#pragma once

#include "ITest.h"
#include <memory>

class MultipleObjectsTest : public AutoTest {
	std::shared_ptr<URM::Engine::SceneObject> mModelsParent;
	int mCurrentObjectCount = 0;


	void AddObject();
	void OnInit(URM::Engine::Engine& engine) override;
	void OnUpdate(URM::Engine::Engine& engine) override;
	bool IncreaseCount(size_t amount) override;
	bool DecreaseCount() override;
	unsigned int GetCount() override;

	float GetTargetCurveMultiplier() override;

public:
	virtual URM::Core::WindowCreationParams GetWindowParams(HINSTANCE instance) const {
		return URM::Core::WindowCreationParams(800, 800, "Multiple objects test", instance);
	}
};