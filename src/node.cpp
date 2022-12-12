#include "node.hpp"

#include <algorithm>

#include "consts.hpp"

void VPTree::createTree(const std::vector<DataPoint> &points) {
  auto vantage_point = getVantagePoint_(points);
}

int getVantagePoint_(const std::vector<DataPoint> &points) {
  auto indices = math::getRandNonRepeatingIntVec(
      0, points.size(),
      VP_CONST::A_SUBSET_CARDINALITY + VP_CONST::B_SUBSET_CARDINALITY);
  // random indices for two sets
  std::vector A_indices(indices.begin(),
                        indices.begin() + VP_CONST::A_SUBSET_CARDINALITY);
  std::vector B_indices(indices.begin() + VP_CONST::A_SUBSET_CARDINALITY,
                        indices.end());

  std::vector<double> distances_vec;
  std::vector<double> variances_vec;
  for (int i = 0; i < VP_CONST::A_SUBSET_CARDINALITY; ++i) {
    distances_vec.empty();
    for (int j = 0; j < VP_CONST::B_SUBSET_CARDINALITY; ++j) {
      auto dist = math::minkowskiDist(points.at(A_indices.at(i)),
                                      points.at(B_indices.at(j)),
                                      VP_CONST::MINKOWSKI_PARAM);
      distances_vec.push_back(dist);
    }
    variances_vec.push_back(math::calculateVariance(distances_vec));
  }
  // finding max variance value ID in vector of variances
  int max_id = std::distance(
      variances_vec.begin(),
      std::max_element(variances_vec.begin(), variances_vec.end()));

  // returning global index of highest variance point
  return A_indices.at(max_id);
}
