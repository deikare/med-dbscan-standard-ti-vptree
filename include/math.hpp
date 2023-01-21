#pragma once
#include <algorithm>
#include <cmath>
#include <experimental/random>
#include <vector>

#include "consts.hpp"

namespace math {
/** @brief Function calculates Mikowski distance
 * @param point1 first point
 * @param point2 second point
 * @param m Minkowski distance parameter
 */
double minkowskiDist(const DataPoint &point1, const DataPoint &point2,
                     double m);

/** @brief Function calculates mean of values in STL vector */
double calculateMean(const std::vector<double> &points);

/** @brief Function calculates variance of values in STL vector */
double calculateVariance(const std::vector<double> &points);

/** @brief Function draws a random non-repeating integer numbers vector from
 * given range
 * @param min minimal number in the range (included in drawing)
 * @param max maximal number in the range (included in drawing)
 * @param size number of wanted values (cannot be bigger than length of
 * [min,max] range)
 * @returns STL vector of random integers from given range (the values are
 * non-repeating)
 */
std::vector<int> getRandNonRepeatingIntVec(int min, int max, int size);

/** @brief Function calculates median of values in STL vector */
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

int factorial(int n);

int binomialCoefficient(int n, int k);

/** @brief Function calculates Rand index
 * @param base_classes reference to real classes given in datafile
 * @param found_classes reference to classes found by an algorithm
 * @returns Rand index value
 */
double calculateRandIndex(const std::vector<int> &base_classes,
                          const std::vector<int> &found_classes);

}  // namespace math
