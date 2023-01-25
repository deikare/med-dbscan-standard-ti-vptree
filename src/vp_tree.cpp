#include "vp_tree.hpp"

#include <algorithm>
#include <iostream>

#include "consts.hpp"

template <typename T>
std::vector<T> concatenateVectors(std::vector<T> A, std::vector<T> B) {
  std::vector<T> concatenated;
  concatenated.reserve(A.size() + B.size());  // preallocate memory
  concatenated.insert(concatenated.end(), A.begin(), A.end());
  concatenated.insert(concatenated.end(), B.begin(), B.end());
  return concatenated;
}

VPTree::VPTree(const std::vector<DataPoint> &points, unsigned long & distanceCalculationCount): distanceCalculationCount(distanceCalculationCount) {
  root_ = std::make_shared<Node>();
  createTree(points);
}

void VPTree::createTree(const std::vector<DataPoint> &points) {
  try {
    std::vector<std::size_t> primary_indices;
    primary_indices.resize(points.size());
    std::iota(primary_indices.begin(), primary_indices.end(), 0);
    createTreeRecursive_(root_, points, primary_indices);
  } catch (std::exception &ex) {
    std::cout << ex.what() << std::endl;
  }
}

void VPTree::createTreeRecursive_(
    std::shared_ptr<Node> sub_root, const std::vector<DataPoint> &points,
    const std::vector<std::size_t> &global_indices) {
  std::vector<DataPoint> processed_points = points;
  auto vantage_point_idx_local = getVantagePointIndex_(processed_points);
  auto vantage_point_idx_global = global_indices.at(vantage_point_idx_local);

  // getDistMedian_ needs to be called before removing vantage point from
  // vector
  auto mu = getDistMedian_(processed_points, vantage_point_idx_local);
  sub_root->id = vantage_point_idx_global;
  sub_root->mu = mu;
  auto splitted_vectors = splitPointsVector_(processed_points, global_indices,
                                             vantage_point_idx_local, mu);

  if (splitted_vectors.leftVector.size() > 0) {
    sub_root->createLeftNode();
    createTreeRecursive_(sub_root->leftChild, splitted_vectors.leftVector,
                         splitted_vectors.leftIndices);
  }
  if (splitted_vectors.rightVector.size() > 0) {
    sub_root->createRightNode();
    createTreeRecursive_(sub_root->rightChild, splitted_vectors.rightVector,
                         splitted_vectors.rightIndices);
  }
}

int VPTree::getVantagePointIndex_(const std::vector<DataPoint> &points) {
  if (points.size() == 1) {
    return 0;
  }
  // if points set is big enough to find two random subsets of given cardinality
  if (points.size() >
      VP_CONST::A_SUBSET_CARDINALITY + VP_CONST::B_SUBSET_CARDINALITY) {
    auto rand_indices = math::getRandNonRepeatingIntVec(
        0, points.size() - 1,
        VP_CONST::A_SUBSET_CARDINALITY + VP_CONST::B_SUBSET_CARDINALITY);
    // random indices for two sets
    std::vector A_indices(
        rand_indices.begin(),
        rand_indices.begin() + VP_CONST::A_SUBSET_CARDINALITY);
    std::vector B_indices(rand_indices.begin() + VP_CONST::A_SUBSET_CARDINALITY,
                          rand_indices.end());

    std::vector<double> distances_vec;
    std::vector<double> variances_vec;
    for (int i = 0; i < VP_CONST::A_SUBSET_CARDINALITY; ++i) {
      distances_vec.clear();
      for (int j = 0; j < VP_CONST::B_SUBSET_CARDINALITY; ++j) {
        auto dist = math::minkowskiDist(points.at(A_indices.at(i)),
                                        points.at(B_indices.at(j)),
                                        VP_CONST::MINKOWSKI_PARAM);
        distanceCalculationCount++;
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
  // alternative when subsets cardinality is too high for points.size()
  std::vector<double> distances_vec;
  std::vector<double> variances_vec;
  for (std::size_t i = 0; i < points.size(); ++i) {
    for (std::size_t j = 0; j < points.size(); ++j) {
      if (i != j) {
        auto dist = math::minkowskiDist(points.at(i), points.at(j),
                                        VP_CONST::MINKOWSKI_PARAM);
        distanceCalculationCount++;
        distances_vec.push_back(dist);
      }
    }
    variances_vec.push_back(math::calculateVariance(distances_vec));
  }
  // finding max variance value ID in vector of variances
  int max_id = std::distance(
      variances_vec.begin(),
      std::max_element(variances_vec.begin(), variances_vec.end()));
  return max_id;
}

double VPTree::getDistMedian_(const std::vector<DataPoint> &points,
                              std::size_t vantage_point_idx_local) {
  if (points.size() == 1) {
    return 0;
  }
  std::vector<double> distances;
  for (std::size_t i = 0; i < points.size(); ++i) {
    if (i != vantage_point_idx_local) {
      auto dist = math::minkowskiDist(points.at(vantage_point_idx_local),
                                      points.at(i), VP_CONST::MINKOWSKI_PARAM);
      distanceCalculationCount++;
      distances.push_back(dist);
    }
  }
  return math::vectorMedian<double>(distances);
}

SplitVector VPTree::splitPointsVector_(
    const std::vector<DataPoint> &points,
    const std::vector<std::size_t> &global_indices,
    std::size_t vantage_point_idx_local, int mu) {
  SplitVector split_vectors;
  if (points.size() == 1) {
    return split_vectors;
  }
  for (std::size_t i = 0; i < points.size(); ++i) {
    if (i != vantage_point_idx_local) {
      auto dist = math::minkowskiDist(points.at(vantage_point_idx_local),
                                      points.at(i), VP_CONST::MINKOWSKI_PARAM);
      distanceCalculationCount++;
      if (dist < mu) {
        split_vectors.leftVector.push_back(points.at(i));
        split_vectors.leftIndices.push_back(global_indices.at(i));
      } else {
        split_vectors.rightVector.push_back(points.at(i));
        split_vectors.rightIndices.push_back(global_indices.at(i));
      }
    }
  }
  return split_vectors;
}

std::vector<std::size_t> VPTree::findNeighbours(
    const std::vector<DataPoint> &points, std::size_t point_id,
    double epsilon) {
  if (point_id > points.size() - 1) {
    throw std::runtime_error("point_id exceeds points size");
  }
  auto neighbours = findNeighboursRecursive_(root_, points, point_id, epsilon);
  return neighbours;
}

std::vector<std::size_t> VPTree::findNeighboursRecursive_(
    std::shared_ptr<Node> sub_root, const std::vector<DataPoint> &points,
    std::size_t point_id, double epsilon) {
  auto dist = math::minkowskiDist(points.at(sub_root->id), points.at(point_id),
                                  VP_CONST::MINKOWSKI_PARAM);
  distanceCalculationCount++;
  std::vector<std::size_t> neighbours;
  if (dist <= epsilon) {
    // operator '<=' because k+NN, if satisfied then current point should be
    // added to k+NN
    neighbours.push_back(sub_root->id);
  }

  // conditions in ifs below are logically reversed conditions from VP tree
  // lectures
  std::vector<std::size_t> left_neighbours;
  if (dist - sub_root->mu < epsilon && sub_root->leftChild != nullptr) {
    // condition to look in the left subtree
    left_neighbours = findNeighboursRecursive_(sub_root->leftChild, points,
                                               point_id, epsilon);
  }
  neighbours = concatenateVectors(neighbours, left_neighbours);

  std::vector<std::size_t> right_neighbours;
  if (sub_root->mu - dist <= epsilon && sub_root->rightChild != nullptr) {
    // condition to look in the right subtree
    right_neighbours = findNeighboursRecursive_(sub_root->rightChild, points,
                                                point_id, epsilon);
  }

  neighbours = concatenateVectors(neighbours, right_neighbours);
  return neighbours;
}