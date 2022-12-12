#pragma once
#include "math.hpp"

class Node {
  Node *leftChild_;
  Node *rightChild_;
  double mu;
};

class VPTree {
  Node root_;
  int getVantagePoint_(const std::vector<DataPoint> &points);

 public:
  void createTree(const std::vector<DataPoint> &points);
};
