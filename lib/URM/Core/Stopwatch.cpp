#include "pch.h"
#include "Stopwatch.h"

namespace URM::Core {
	void Stopwatch::PreallocateTimePoints(size_t num) {
		this->mTimePoints.resize(num);
	}
	
	void Stopwatch::Start() {
		mStartTime = std::chrono::high_resolution_clock::now();
	}

	void Stopwatch::Stop() {
		mEndTime = std::chrono::high_resolution_clock::now();
	}

	StopwatchResult Stopwatch::GetResult() const {
		auto elapsed = this->GetElapsed();
		if (!this->mStartTime.has_value())
			return StopwatchResult::Empty();
		
		auto timePoints = std::vector<StopwatchResult::TimePointEntry>();
		timePoints.reserve(this->mTimePoints.size());
		for (auto& tp : this->mTimePoints) {
			timePoints.push_back(tp.ToResultEntry(this->mStartTime.value()));
		}

		return StopwatchResult(elapsed, timePoints);
	}
	void Stopwatch::AddPoint(const std::string& name) {
		auto now = std::chrono::high_resolution_clock::now();
		if (!mDisablePreallocateWarning && this->mTimePoints.size() >= this->mTimePoints.capacity()) {
			spdlog::warn("Stopwatch time points capacity exceeded, consider pre-allocating. This warning will be shown only once.");
			this->mDisablePreallocateWarning = true;
		}
		this->mTimePoints.push_back({name, now});
	}

	void Stopwatch::Reset() {
		this->mEndTime = std::nullopt;
		this->mTimePoints.clear();
		this->mStartTime = std::chrono::high_resolution_clock::now();
	}

	std::chrono::duration<double> Stopwatch::GetElapsed() const {
		const TimePoint endTime = this->mEndTime.has_value() ? this->mEndTime.value() : std::chrono::high_resolution_clock::now();

		if (!this->mStartTime.has_value())
			return std::chrono::duration<double>::zero();

		return endTime - mStartTime.value();
	}
}
