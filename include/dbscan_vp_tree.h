//
// Created by deikare on 25.01.23.
//

#ifndef DBSCAN_DBSCAN_VP_TREE_H
#define DBSCAN_DBSCAN_VP_TREE_H

#include "dbscan.hpp"
#include "vp_tree.hpp"


class DBScanVPTree : public DBScan {
private:
    VPTree * tree;

public:
    DBScanVPTree(const std::vector<DataPoint> &points, unsigned long minPts, double eps);

    virtual ~DBScanVPTree();
};


#endif //DBSCAN_DBSCAN_VP_TREE_H
