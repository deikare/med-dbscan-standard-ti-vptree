#include "node.hpp"

#include <algorithm>

#include "consts.hpp"

void VPTree::createTree(const std::vector<DataPoint> &points) {
  createRecursiveTree_(root_, points);
}

void VPTree::createRecursiveTree_(Node *sub_root,
                                  const std::vector<DataPoint> &points) {
  std::vector<DataPoint> processed_points = points;
  auto vantage_point_idx = getVantagePointIndex_(processed_points);

  // getDistMedian_ needs to be called before removing vantage point from vector
  auto mu = getDistMedian_(processed_points, vantage_point_idx);
  sub_root->id = vantage_point_idx;
  sub_root->mu = mu;
  auto splitted_vectors =
      splitPointsVector_(processed_points, vantage_point_idx, mu);
  processed_points.erase(processed_points.begin() + vantage_point_idx);

  if (splitted_vectors.leftVector.size() > 0)
    createRecursiveTree_(sub_root->leftChild, splitted_vectors.leftVector);
  if (splitted_vectors.rightVector.size() > 0)
    createRecursiveTree_(sub_root->rightChild, splitted_vectors.rightVector);
}

int VPTree::getVantagePointIndex_(const std::vector<DataPoint> &points) {
  auto indices = math::getRandNonRepeatingIntVec(
      0, points.size() - 1,
      VP_CONST::A_SUBSET_CARDINALITY + VP_CONST::B_SUBSET_CARDINALITY);
  // random indices for two sets
  std::vector A_indices(indices.begin(),
                        indices.begin() + VP_CONST::A_SUBSET_CARDINALITY);
  std::vector B_indices(indices.begin() + VP_CONST::A_SUBSET_CARDINALITY,
                        indices.end());

  std::vector<double> distances_vec;
  std::vector<double> variances_vec;
  for (int i = 0; i < VP_CONST::A_SUBSET_CARDINALITY; ++i) {
    distances_vec.clear();
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

  // returning global (primary) index of highest variance point
  return A_indices.at(max_id);
}

double VPTree::getDistMedian_(const std::vector<DataPoint> &points,
                              int vantage_point_idx) {
  std::vector<double> distances;
  for (int i = 0; i < points.size(); ++i) {
    if (i != vantage_point_idx) {
      auto dist = math::minkowskiDist(points.at(vantage_point_idx),
                                      points.at(i), VP_CONST::MINKOWSKI_PARAM);
      distances.push_back(dist);
    }
  }
  return math::vectorMedian<double>(distances);
}

SplitVector VPTree::splitPointsVector_(const std::vector<DataPoint> &points,
                                       int vantage_point_idx, int mu) {
  SplitVector split_vectors;
  for (int i = 0; i < points.size(); ++i) {
    if (i != vantage_point_idx) {
      auto dist = math::minkowskiDist(points.at(vantage_point_idx),
                                      points.at(i), VP_CONST::MINKOWSKI_PARAM);
      if (dist < mu) {
        split_vectors.leftVector.push_back(points.at(i));
        split_vectors.leftIndices.push_back(i);
      } else {
        split_vectors.rightVector.push_back(points.at(i));
        split_vectors.rightIndices.push_back(i);
      }
    }
  }
  return split_vectors;
}
