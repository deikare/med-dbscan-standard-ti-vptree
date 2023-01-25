//
// Created by deikare on 25.01.23.
//

#include "dbscan_vp_tree.h"

DBScanVPTree::DBScanVPTree(const std::vector<DataPoint> &points, unsigned long minPts, double eps) : DBScan(points,
                                                                                                            minPts,
                                                                                                            eps) {
    auto startTime = std::chrono::high_resolution_clock::now();
    tree = new VPTree(points, distanceCalculationCount);
    treeCreationDuration = std::chrono::high_resolution_clock::now() - startTime;

    auto neighboursHandler = [this, points](const DataPoint &point) {
        auto startTime = std::chrono::high_resolution_clock::now();
        auto index = pointStatistics.find(point)->second.id;
        auto indexSet =  tree->findNeighbours(points, index, this->eps);
        std::set<DataPoint> result;
        for (auto entry : indexSet) {
            result.emplace(points[entry]);
        }
        neighbourhoodDuration += std::chrono::high_resolution_clock::now() - startTime;
        return result;
    };

    performClustering(points, neighboursHandler);
}

DBScanVPTree::~DBScanVPTree() {
    delete tree;
}

std::string DBScanVPTree::produceStatFileContents(const std::string &datafileName, const std::vector<long> &classes) {
    auto result =  DBScan::produceStatFileContents(datafileName, classes);
    result += "S1: " + std::to_string(VP_CONST::A_SUBSET_CARDINALITY) + ", S2: " + std::to_string(VP_CONST::B_SUBSET_CARDINALITY) += "\n";
    result += "tree creation time: " + std::to_string(treeCreationDuration.count()) + "\n";
    result += "mean distanceCalculationCount: " + std::to_string(distanceCalculationCount);
    return result;
}

void DBScanVPTree::generateOutFile(const std::string &prefix, const std::string &datafileName) {
    DBScan::generateOutFile(prefix, datafileName, "VP-DBSCAN");
}

void DBScanVPTree::generateStatFile(const std::string &prefix, const std::string &datafileName,
                                    const std::vector<long> &classes) {
    DBScan::generateStatFile(prefix, datafileName, "VP-DBSCAN", classes);
}
