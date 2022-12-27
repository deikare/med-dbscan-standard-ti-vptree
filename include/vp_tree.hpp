#pragma once
#include <memory>

#include "math.hpp"

/** @brief Class stores data of separate nodes of VP tree */
class Node {
 public:
  size_t id;  // global index of the point
  double mu;
  std::shared_ptr<Node> leftChild;
  std::shared_ptr<Node> rightChild;
  void createLeftNode() { leftChild = std::make_shared<Node>(); };
  void createRightNode() { rightChild = std::make_shared<Node>(); };
};

/** @brief Struct is used for convenient data returning */
struct SplitVector {
  std::vector<DataPoint> leftVector;
  std::vector<DataPoint> rightVector;
  std::vector<std::size_t> leftIndices;   // stores global left indices
  std::vector<std::size_t> rightIndices;  // stores global right indices
};

/** @brief Class stores VP tree and allows for searching nearest
 * neighbours, the class doesn't store data vector itself but just operates on
 * indices of data entries */
class VPTree {
  std::shared_ptr<Node> root_;
  /** @brief Method returns local vantage point index */
  int getVantagePointIndex_(const std::vector<DataPoint> &points);

  /** @brief Method calculates median of distances between point of given id and
   * other points from the dataset */
  double getDistMedian_(const std::vector<DataPoint> &points,
                        std::size_t vantage_point_idx_local);

  /** @brief Method divides the dataset into two seprate subsets pairs (values,
   * indices) based on relation points Minkowski distance to vantage point and
   * calculated distance median */
  SplitVector splitPointsVector_(const std::vector<DataPoint> &points,
                                 const std::vector<std::size_t> &global_indices,
                                 std::size_t vantage_point_idx_local, int mu);

  /** @brief Helper recursion method for VP tree creation */
  void createTreeRecursive_(std::shared_ptr<Node> sub_root,
                            const std::vector<DataPoint> &points,
                            const std::vector<std::size_t> &global_indices);

  /** @brief Helper recursion method for finding nearest neighbours */
  std::vector<std::size_t> findNeighboursRecursive_(
      std::shared_ptr<Node> sub_root, const std::vector<DataPoint> &points,
      std::size_t point_id, double epsilon);

 public:
  /** @brief Constructor that creates VP tree for given points */
  VPTree(const std::vector<DataPoint> &points);

  /** @brief Method allows for recreation of VP tree for another set of data
   * @param points is dataset that will be used for creation of VP tree
   */
  void createTree(const std::vector<DataPoint> &points);

  /** @brief Method allows for finding all nearest neighbours (k-NN+) in given
   * distance
   * @param points values of dataset
   * @param point_id index of point which neighbours are searched for
   * tree
   * @param epsilon is the "radius" of Minkowski distance inside which points
   * are regarded as neighbours
   */
  std::vector<std::size_t> findNeighbours(const std::vector<DataPoint> &points,
                                          std::size_t point_id, double epsilon);
};
