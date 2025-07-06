#pragma once

#include "ITest.h"

// This test measures the throughput of object transformations in the scene graph.
// It creates a hierarchy of objects and applies rotation transformation to the parent object, 
// Which updates the transformations of all child objects.
class ObjectsTransformThroughputTest : public AutoTest {
	int mElementsPerLevel = 1000;

	std::shared_ptr<URM::Engine::SceneObject> mObjectsParent;
	std::shared_ptr<URM::Engine::SceneObject> mLastObject;
	int mCurrentObjectCount = 0;

	// Inherited via AutoTest
	void OnInit(URM::Engine::Engine& engine) override;
	void OnUpdate(URM::Engine::Engine& engine) override;
	bool IncreaseCount(size_t amount) override;
	bool DecreaseCount() override;
	unsigned int GetCount() override;

	float GetTargetCurveMultiplier() override;
public:
	ObjectsTransformThroughputTest(int elementsPerLevel = 5000) {
		this->mElementsPerLevel = elementsPerLevel;
	}

	virtual URM::Core::WindowCreationParams GetWindowParams(HINSTANCE instance) const {
		return URM::Core::WindowCreationParams(256, 256, "Objects transform throughput test", instance);
	}
};