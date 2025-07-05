#pragma once

#include <vector>
#include <optional>
#include <algorithm>

class AverageAccumulator {
	int currentIndex = 0;
	std::vector<double> mValues;

	double Percentile(double* sortedData, int n, double p) const {
		assert(p >= 0 && p <= 100);

		auto rank = (p / 100) * (n - 1) + 1;
		auto floorRank = static_cast<int>(rank);
		auto ceilRank = floorRank + 1;

		auto floorValue = sortedData[floorRank - 1];
		if (ceilRank >= n) {
			return floorValue;
		}
		auto ceilValue = sortedData[ceilRank - 1];
		auto interpolatedValue = floorValue + (ceilValue - floorValue) * (rank - floorRank);

		return interpolatedValue;
	}

public:
	bool IsFilled() const {
		return currentIndex >= mValues.size();
	}

	double GetAverage() const {
		double sum = 0.0;
		for (const auto& value : mValues) {
			sum += value;
		}
		return sum / mValues.size();
	}

	// Remove outliers using the IQR method
	std::optional<double> GetAverageWithoutOutliers(double sensitivity = 1.5) {
		std::sort(mValues.begin(), mValues.end());

		auto q1 = Percentile(mValues.data(), mValues.size(), 25);
		auto q2 = Percentile(mValues.data(), mValues.size(), 50);
		auto q3 = Percentile(mValues.data(), mValues.size(), 75);

		auto lowerBound = q1 - sensitivity * (q3 - q1);
		auto upperBound = q3 + sensitivity * (q3 - q1);

		double sum = 0.0;
		int count = 0;
		for (const auto& value : mValues) {
			if (value >= lowerBound && value <= upperBound) {
				sum += value;
				count++;
			}
			else {
				spdlog::trace("[AverageAccumulator] Value {} is an outlier, ignoring it.", value);
			}
		}

		if (count == 0) {
			return std::nullopt; // No valid values to calculate average
		}

		return sum / count; // Return the average of valid values
	}

	void AddValue(double value) {
		if(currentIndex >= mValues.size()) {
			spdlog::warn("[AverageAccumulator] Adding value to full accumulator, ignoring next values.");
			return;
		}

		mValues[currentIndex++] = value;
	}

	void Clear() {
		currentIndex = 0;
	}

	AverageAccumulator(size_t size) : mValues(size) {}
};