#pragma once
#include <cmath>
#include <experimental/random>
#include <vector>

#include "consts.hpp"

namespace math {
double minkowskiDist(const DataPoint &point1, const DataPoint &point2,
                     double m);

double calculateMean(const std::vector<double> &points);

double calculateVariance(const std::vector<double> &points);

std::vector<int> getRandNonRepeatingIntVec(int min, int max, int size);
}  // namespace math
