#pragma once
#include <memory>

#include "math.hpp"

class Node {
public:
  int id;
  double mu;
  std::shared_ptr<Node> leftChild;
  std::shared_ptr<Node> rightChild;
};

struct SplitVector {
  std::vector<DataPoint> leftVector;
  std::vector<DataPoint> rightVector;
  std::vector<int> leftIndices;   // stores global left indices
  std::vector<int> rightIndices;  // stores global right indices
};

class VPTree {
  std::shared_ptr<Node> root_;
  int getVantagePointIndex_(const std::vector<DataPoint> &points,
                            const std::vector<int> &global_indices);
  double getDistMedian_(const std::vector<DataPoint> &points,
                        int vantage_point_idx);
  SplitVector splitPointsVector_(const std::vector<DataPoint> &points,
                                 int vantage_point_idx, int mu);
  void createRecursiveTree_(std::shared_ptr<Node> sub_root,
                            const std::vector<DataPoint> &points,
                            const std::vector<int> &global_indices);

 public:
  VPTree();
  void createTree(const std::vector<DataPoint> &points);
};
