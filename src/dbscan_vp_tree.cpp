//
// Created by deikare on 25.01.23.
//

#include "dbscan_vp_tree.h"

DBScanVPTree::DBScanVPTree(const std::vector<DataPoint> &points, unsigned long minPts, double eps) : DBScan(points,
                                                                                                            minPts,
                                                                                                            eps) {
    tree = new VPTree(points);

    auto neighboursHandler = [this, points](const DataPoint &point) {
        auto index = pointStatistics.find(point)->second.id;
        auto indexSet =  tree->findNeighbours(points, index, this->eps);
        std::set<DataPoint> result;
        for (auto entry : indexSet) {
            result.emplace(points[entry]);
        }
        return result;
    };

    performClustering(points, neighboursHandler);
}

DBScanVPTree::~DBScanVPTree() {
    delete tree;
}
