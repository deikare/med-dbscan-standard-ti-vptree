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

    std::chrono::high_resolution_clock::duration treeCreationDuration = {};
    unsigned long distanceCalculationCount = 0;

public:
    DBScanVPTree(const std::vector<DataPoint> &points, unsigned long minPts, double eps);

    virtual ~DBScanVPTree();protected:
    std::string produceStatFileContents(const std::string &datafileName, const std::vector<long> &classes) override;

public:
    void generateOutFile(const std::string &prefix, const std::string &datafileName) override;

    void generateStatFile(const std::string &prefix, const std::string &datafileName,
                          const std::vector<long> &classes) override;


};


#endif //DBSCAN_DBSCAN_VP_TREE_H
