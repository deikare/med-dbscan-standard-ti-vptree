#pragma once
#include "math.hpp"

struct Node {
  int id;
  double mu;
  Node *leftChild;
  Node *rightChild;
};

struct SplitVector {
  std::vector<DataPoint> leftVector;
  std::vector<DataPoint> rightVector;
  std::vector<int> leftIndices;   // stores global left indices
  std::vector<int> rightIndices;  // stores global right indices
};

class VPTree {
  Node *root_;
  int getVantagePointIndex_(const std::vector<DataPoint> &points);
  double getDistMedian_(const std::vector<DataPoint> &points,
                        int vantage_point_idx);
  SplitVector splitPointsVector_(const std::vector<DataPoint> &points,
                                 int vantage_point_idx, int mu);
  void createRecursiveTree_(Node *sub_root,
                            const std::vector<DataPoint> &points);

 public:
  void createTree(const std::vector<DataPoint> &points);
};
