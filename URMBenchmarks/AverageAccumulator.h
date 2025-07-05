#pragma once

#include <vector>
#include <optional>

class AverageAccumulator {
	bool isFilledOnce = false;
	int currentIndex = 0;
	std::vector<double> mValues;

public:
	std::optional<double> GetAverage() const {
		if (!isFilledOnce) {
			return std::nullopt; // Not enough data to calculate average
		}

		double sum = 0.0;
		for (const auto& value : mValues) {
			sum += value;
		}
		return sum / mValues.size();
	}

	void AddValue(double value) {
		mValues[currentIndex++] = value;
		if(currentIndex >= mValues.size()) {
			currentIndex = 0;
			isFilledOnce = true;
		}
	}

	void Clear() {
		isFilledOnce = false;
		currentIndex = 0;
	}

	AverageAccumulator(size_t size) : mValues(size) {}
};