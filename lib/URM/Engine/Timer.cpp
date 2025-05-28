#include "pch.h"
#include "Timer.h"

namespace URM::Engine {
	void Timer::Update() {
		auto now = std::chrono::high_resolution_clock::now();
		if (!this->startTime.has_value()) {
			this->startTime = now;
		}
		this->mElapsedTime = std::chrono::duration<float>(now - this->startTime.value()).count();

		float newDeltaTime = 0;
		if (this->lastUpdate.has_value()) {
			newDeltaTime = std::chrono::duration<float>(now - this->lastUpdate.value()).count();
		}
		this->lastUpdate = now;
		this->mDeltaTime = newDeltaTime;
	}
}
