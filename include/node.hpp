#pragma once
#include "math.hpp"

class Node {
  Node *leftChild_;
  Node *rightChild_;
  double mu;
};

class VPTree {
  Node root_;
  std::vector<DataPoint> getRandomSubset_(const std::vector<DataPoint> &points,
                                          int cardinality);

 public:
  void createTree(const std::vector<DataPoint> &points);
};
