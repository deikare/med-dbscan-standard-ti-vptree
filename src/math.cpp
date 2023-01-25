#include "math.hpp"

#include <stdexcept>

double math::minkowskiDist(const DataPoint &point1, const DataPoint &point2,
                           double m) {
  double sum = 0;
  for (std::size_t i = 0; i < point1.size(); ++i) {
    sum += std::abs(std::pow((point1[i] - point2[i]), m));
  }
  sum = std::pow(sum, 1. / m);
  return sum;
}

double math::calculateMean(const std::vector<double> &points) {
  double mean;
  for (auto point : points) {
    mean += point;
  }
  mean /= points.size();
  return mean;
}

double math::calculateVariance(const std::vector<double> &points) {
  double mean = calculateMean(points);
  double variance;
  for (std::size_t i = 0; i < points.size(); ++i) {
    variance += pow(points[i] - mean, 2);
  }
  variance /= points.size();
  return variance;
}

std::vector<int> math::getRandNonRepeatingIntVec(int min, int max, int size) {
  if (!(max - min > size - 1)) throw std::runtime_error("sizes not correct");
  std::vector<int> rand_vec;
  int num;
  for (int i = 0; i < size; ++i) {
    num = std::experimental::randint(min, max);
    if (std::find(rand_vec.begin(), rand_vec.end(), num) == rand_vec.end()) {
      rand_vec.push_back(num);
    } else {
      i--;
    }
  }
  return rand_vec;
}

int math::factorial(int n) {
  int factorial = 1;
  for (int i = 1; i <= n; i++) {
    factorial *= i;
  }
  return factorial;
}

int math::binomialCoefficient(int n, int k) {
  if (n < k) throw std::runtime_error("binomial params not correct");
  return factorial(n) / (factorial(k) * factorial(n - k));
}

double math::calculateRandIndex(const std::vector<int> &base_classes,
                                const std::vector<int> &found_classes) {
  int TP = 0;
  int TN = 0;
  int base_group = 0;
  int found_group = 0;
  for (size_t i = 0; i < base_classes.size(); ++i) {
    base_group = base_classes.at(i);
    found_group = found_classes.at(i);
    for (size_t j = i + 1; j < found_classes.size(); ++j) {
      if (base_group == base_classes.at(j) &&
          found_group == found_classes.at(j)) {
        TP++;
      }
      if (base_group != base_classes.at(j) &&
          found_group != found_classes.at(j)) {
        TN++;
      }
    }
  }
  double rand = (TP + TN) / (double)binomialCoefficient(base_classes.size(), 2);
  return rand;
}
