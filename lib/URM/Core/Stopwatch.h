#pragma once

#include <chrono>
#include <vector>
#include <optional>

namespace URM::Core {
	struct StopwatchResult {
		struct TimePointEntry {
			std::string name;
			std::chrono::duration<double> elapsed;
		};
	
		std::chrono::duration<double> elapsed;
		std::vector<TimePointEntry> timePoints;

		std::string ToString(const std::string& name) const {
			std::stringstream ss;
			const auto elapsedMs = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / 1000.0f;
			ss << name << ": " << elapsedMs << " ms\n";
			for (auto& tp : this->timePoints) {
				ss << "\t- " << tp.name << ": " << std::chrono::duration_cast<std::chrono::microseconds>(tp.elapsed).count() / 1000.0f << " ms\n";
			}
		
			return ss.str();
		}
	
		static StopwatchResult Empty() {
			return StopwatchResult(std::chrono::duration<double>::zero(), {});
		}

		StopwatchResult(const std::chrono::duration<double> elapsed, const std::vector<TimePointEntry>& timePoints) : elapsed(elapsed), timePoints(timePoints) {}
	};

	class Stopwatch {
		using TimePoint = std::chrono::high_resolution_clock::time_point;
	
		struct TimePointEntry {
			std::string name;
			TimePoint time;

			[[nodiscard]]
			StopwatchResult::TimePointEntry ToResultEntry(TimePoint startPoint) const {
				return {name, std::chrono::duration_cast<std::chrono::duration<double>>(time - startPoint)};
			}
		};

		std::optional<TimePoint> mStartTime = std::nullopt;
		std::optional<TimePoint> mEndTime = std::nullopt;
		std::vector<TimePointEntry> mTimePoints;

		bool mDisablePreallocateWarning = false;
	public:
		void PreallocateTimePoints(size_t num);
		
		void Start();
		void Stop();
		void Reset();
	
		std::chrono::duration<double> GetElapsed() const;
		[[nodiscard]] StopwatchResult GetResult() const;

		void AddPoint(const std::string& name);
	};

}
