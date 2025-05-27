#include "pch.h"
#include "Timer.h"

namespace URM::Engine {
    void Timer::Update() {
        auto now = std::chrono::high_resolution_clock::now();
        if (!this->startTime.has_value()) {
            this->startTime = now;
        }
        this->elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - this->startTime.value()).count() / 1000000.0f;

        float newDeltaTime = 0;
        if (this->lastUpdate.has_value()) {
            newDeltaTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - this->lastUpdate.value()).count() / 1000000.0f;
        }
        this->lastUpdate = now;
        this->deltaTime = newDeltaTime;
    }
}