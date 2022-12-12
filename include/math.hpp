#pragma once
#include <cmath>
#include <vector>

#include "consts.hpp"

using DataPoint = std::vector<double>;

double minkowskiDist(const DataPoint &point1, const DataPoint &point2,
                     double m) {
  double sum = 0;
  for (int i = 0; i < point1.size(); ++i) {
    sum += abs(pow((point1[i] - point2[i]), m));
  }
  sum = pow(sum, 1. / m);
  return sum;
}

double calculateMean(const std::vector<double> &points) {
  double mean;
  for (auto point : points) {
    mean += point;
  }
  mean /= points.size();
  return mean;
}

double calculateVariance(const std::vector<double> &points) {
  double mean = calculateMean(points);
  double variance;
  for (int i = 0; i < points.size(); ++i) {
    variance += pow(points[i] - mean, 2);
  }
  variance /= points.size();
  return variance;
}