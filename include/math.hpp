#pragma once
#include <algorithm>
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

template <typename T>
double vectorMedian(std::vector<T> &v) {
  if (v.empty()) {
    return 0.0;
  }
  auto n = v.size() / 2;
  std::nth_element(v.begin(), v.begin() + n, v.end());
  auto med = v[n];
  if (v.size() % 2 == 0) {  // If the set size is even
    auto max_it = std::max_element(v.begin(), v.begin() + n);
    med = (*max_it + med) / 2.0;
  }
  return med;
}
}  // namespace math
