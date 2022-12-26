#pragma once
#include <memory>

#include "math.hpp"

class Node {
 public:
  int id;  // global index of the point
  double mu;
  std::shared_ptr<Node> leftChild;
  std::shared_ptr<Node> rightChild;
  void createLeftNode() { leftChild = std::make_shared<Node>(); };
  void createRightNode() { rightChild = std::make_shared<Node>(); };
};

struct SplitVector {
  std::vector<DataPoint> leftVector;
  std::vector<DataPoint> rightVector;
  std::vector<std::size_t> leftIndices;   // stores global left indices
  std::vector<std::size_t> rightIndices;  // stores global right indices
};

class VPTree {
  std::shared_ptr<Node> root_;
  /** @brief Method returns local vantage point index */
  int getVantagePointIndex_(const std::vector<DataPoint> &points);
  double getDistMedian_(const std::vector<DataPoint> &points,
                        std::size_t vantage_point_idx_local);
  SplitVector splitPointsVector_(const std::vector<DataPoint> &points,
                                 const std::vector<std::size_t> &global_indices,
                                 std::size_t vantage_point_idx_local, int mu);
  void createTreeRecursive_(std::shared_ptr<Node> sub_root,
                            const std::vector<DataPoint> &points,
                            const std::vector<std::size_t> &global_indices);

 public:
  VPTree(const std::vector<DataPoint> &points);
  void createTree(const std::vector<DataPoint> &points);
};
