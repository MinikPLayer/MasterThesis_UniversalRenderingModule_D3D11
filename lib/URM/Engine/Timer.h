#pragma once
#include <optional>
#include <chrono>

namespace URM::Engine {
	class Timer {
		friend class Engine;

		std::optional<std::chrono::high_resolution_clock::time_point> lastUpdate = std::nullopt;
		std::optional<std::chrono::high_resolution_clock::time_point> startTime = std::nullopt;

		float deltaTime = 0;
		float elapsedTime = 0;

		void Update();

	public:
		float GetDeltaTime() {
			return deltaTime;
		}

		float GetElapsedTime() {
			return elapsedTime;
		}
	};
}