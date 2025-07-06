#pragma once

#include "ITest.h"
#include <URM/Core/Utils.h>

// Warning: First tests show no real limitations on pixel throughput with a single object.
class PixelThroughputTest : public AutoTest {
	bool mResolutionChanged = false;

	URM::Core::Size2i mCurrentWindowSize = { 400, 400 };

	unsigned int GetTargetFPS() override {
		return 1000;
	}

	void OnInit(URM::Engine::Engine& engine) override;
	void OnUpdate(URM::Engine::Engine& engine) override;
	bool IncreaseCount(size_t amount) override;
	bool DecreaseCount() override;
	unsigned int GetCount() override;
	unsigned int GetScore() override;

public:
	virtual URM::Core::WindowCreationParams GetWindowParams(HINSTANCE instance) const {
		return URM::Core::WindowCreationParams(mCurrentWindowSize.width, mCurrentWindowSize.height, "Pixel throughput test", instance);
	}
};