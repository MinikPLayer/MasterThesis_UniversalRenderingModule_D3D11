#pragma once
#include <URM/Engine/Engine.h>
#include <URM/Core/Stopwatch.h>
#include <chrono>
#include <vector>

#include "AverageAccumulator.h"

#undef max

class ITest {
protected:
	virtual void OnInit(URM::Engine::Engine& engine) = 0;
	virtual void OnUpdate(URM::Engine::Engine& engine) = 0;

public:
	virtual URM::Core::WindowCreationParams GetWindowParams(HINSTANCE instance) const {
		return URM::Core::WindowCreationParams(1280, 720, "URM Benchmarks", instance);
	}

	virtual void Init(URM::Engine::Engine& engine) {
		OnInit(engine);
	}

	virtual void Update(URM::Engine::Engine& engine) {
		OnUpdate(engine);
	}

	virtual ~ITest() = default;
};

class AutoTest : public ITest {
	AverageAccumulator mAverageAccumulator;

	int targetHitCount = 0;

	double mTargetFpsFrameTime = 1.0 / 55.0; // 55 FPS
	URM::Core::Stopwatch mStopwatch;

	bool mIsDone = false;
	bool mWasBelowTargetFrameTime = false;

	std::chrono::time_point<std::chrono::high_resolution_clock> lastPrint = std::chrono::high_resolution_clock::now();
protected:

	virtual bool IncreaseCount(size_t amount) = 0;
	virtual bool DecreaseCount() = 0;

	// This multiplier is used to reduce the increase rate of the count when running faster than the target FPS.
	virtual float GetTargetCurveMultiplier() {
		return 0.5f;
	}

	virtual unsigned int GetTargetFPS() {
		return 60;
	}

	virtual unsigned int GetCount() = 0;
	virtual unsigned int GetScore() {
		return GetCount();
	}

	void FinishTest(bool success) {
		auto score = GetScore();
		MessageBox(nullptr, URM::Core::StringUtils::StringToWString(fmt::format("Test score: {}\nTarget FPS: {}", score, GetTargetFPS())).c_str(), L"Test Complete", MB_OK | MB_ICONINFORMATION);
		auto clipboardData = fmt::format("{}", score);

		OpenClipboard(nullptr);
		EmptyClipboard();
		HGLOBAL hClipboardData = GlobalAlloc(GMEM_DDESHARE, (clipboardData.size() + 1) * sizeof(char));
		char* pchData = static_cast<char*>(GlobalLock(hClipboardData));
		strcpy_s(pchData, clipboardData.size() + 1, clipboardData.c_str());
		GlobalUnlock(hClipboardData);
		SetClipboardData(CF_TEXT, hClipboardData);
		CloseClipboard();
	}

public:

	void Init(URM::Engine::Engine& engine) override {
		ITest::Init(engine);
		mTargetFpsFrameTime = 1.0 / GetTargetFPS();
	}

	void Update(URM::Engine::Engine& engine) override {
		ITest::Update(engine);
		mStopwatch.Stop();
		auto result = mStopwatch.GetElapsed();

		if (mIsDone) {
			return;
		}

		if (result.count() > 0) {
			mAverageAccumulator.AddValue(result.count());
			if (mAverageAccumulator.IsFilled()) {
				auto averageFrameTime = mAverageAccumulator.GetAverageWithoutOutliers(5.0);
				if (!averageFrameTime.has_value()) {
					spdlog::warn("[IAutoTest] Average frame time is not available, skipping frame time check.");
					return;
				}

				mAverageAccumulator.Clear();
				auto now = std::chrono::high_resolution_clock::now();
				if(std::chrono::duration_cast<std::chrono::seconds>(now - lastPrint).count() > 0.5) {
					lastPrint = now;
					spdlog::info("[IAutoTest] Current FPS: {:.2f}. Current count: {}\n", 1.0 / averageFrameTime.value(), GetCount());
				}

				OutputDebugString(URM::Core::StringUtils::StringToWString(fmt::format("{}\n", averageFrameTime.value())).c_str());

				// Running faster than the target FPS, increase the count
				if (!mWasBelowTargetFrameTime && averageFrameTime < mTargetFpsFrameTime) {
					auto increaseRate = (mTargetFpsFrameTime / averageFrameTime.value()) - 1.0;
					increaseRate *= GetTargetCurveMultiplier(); // Reduce the increase rate to avoid overshooting
					auto increaseCount = std::ceil(increaseRate * GetCount());
					if (!IncreaseCount(increaseCount)) {
						spdlog::info("[IAutoTest] Cannot increase count, stopping test.");
						mIsDone = true;
						FinishTest(false);
					}

					//spdlog::info("[IAutoTest] Running faster than target FPS, increasing count by {} ({:.1f}%) to {}. Current average frame time: {:.3f} ms", 
					//	increaseCount, 100 * increaseRate, GetCount(), averageFrameTime.value() * 1000.0);
					targetHitCount = 0;
				}
				// Running slower than the min FPS, decrease the count
				else if(averageFrameTime > mTargetFpsFrameTime) {
					if (!DecreaseCount()) {
						//spdlog::info("[IAutoTest] Cannot decrease count, stopping test.");
						mIsDone = true;
						FinishTest(false);
					}

					targetHitCount = 0;
					mWasBelowTargetFrameTime = true;
				}
				else {
					if (targetHitCount++ >= 3) {
						auto score = GetScore();
						spdlog::info("[IAutoTest] Target FPS reached, stopping test.\nScore: {}", score);
						mIsDone = true;

						FinishTest(true);
					}
				}
			}
		}

		mStopwatch.Reset();
	}

	AutoTest() 
		: mAverageAccumulator(100)
	{}
};