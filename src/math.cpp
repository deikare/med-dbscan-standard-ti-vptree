#include "math.hpp"

#include <stdexcept>

double math::minkowskiDist(const DataPoint &point1, const DataPoint &point2,
                           double m) {
  double sum = 0;
  for (int i = 0; i < point1.size(); ++i) {
    sum += abs(pow((point1[i] - point2[i]), m));
  }
  sum = pow(sum, 1. / m);
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
  for (int i = 0; i < points.size(); ++i) {
    variance += pow(points[i] - mean, 2);
  }
  variance /= points.size();
  return variance;
}

std::vector<int> math::getRandNonRepeatingIntVec(int min, int max, int size) {
  if (size - 1 > max - min) throw std::runtime_error("sizes not correct");
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
