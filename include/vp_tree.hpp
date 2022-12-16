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
  std::vector<int> leftIndices;   // stores global left indices
  std::vector<int> rightIndices;  // stores global right indices
};

class VPTree {
  std::shared_ptr<Node> root_;
  /** @brief Method returns local vantage point index */
  int getVantagePointIndex_(const std::vector<DataPoint> &points);
  double getDistMedian_(const std::vector<DataPoint> &points,
                        int vantage_point_idx_local);
  SplitVector splitPointsVector_(const std::vector<DataPoint> &points,
                                 const std::vector<int> &global_indices,
                                 int vantage_point_idx_local, int mu);
  void createRecursiveTree_(std::shared_ptr<Node> sub_root,
                            const std::vector<DataPoint> &points,
                            const std::vector<int> &global_indices);

 public:
  VPTree(const std::vector<DataPoint> &points);
  void createTree(const std::vector<DataPoint> &points);
};
