//
// Created by deikare on 05.01.23.
//
#include "dbscan.hpp"

std::map<DataPoint, unsigned long> dbscan(const std::vector<DataPoint> & points, const std::function<double(DataPoint, DataPoint)>& distanceHandler, const double eps, const unsigned int minPts) {
    std::map<DataPoint, unsigned long> result;
    unsigned long clusterIndex = NOISE;

    for (const auto& point : points) {
        if (result.find(point) == result.end()) {
            auto neighbourSet = neighbours(point, points, distanceHandler, eps);
            if (neighbourSet.size() < minPts) {
                result.emplace(point, NOISE);
                continue;
            }

            clusterIndex++;
            result.emplace(point, clusterIndex);
            neighbourSet.erase(point);
            for (const auto& neighbour: neighbourSet) {
                auto label = result.find(neighbour);
                if (label != result.end()) {
                    if (label->second == NOISE)
                        label->second = clusterIndex;
                }
                else {
                    result.emplace(neighbour, clusterIndex);
                    auto neighboursOfNeighbour = neighbours(neighbour, points, distanceHandler, eps);
                    if (neighboursOfNeighbour.size() >= minPts)
                        neighbourSet.merge(neighboursOfNeighbour);
                }
            }
        }
    }

    return result;
}

std::set<DataPoint> neighbours(const DataPoint &point, const std::vector<DataPoint> &points,
                               const std::function<double(const DataPoint&, const DataPoint&)>& distanceHandler, double eps) {
    std::set<DataPoint> result;
    for (const auto& neighbourWannabe : points) {
        if (distanceHandler(neighbourWannabe, point) <= eps)
            result.emplace(neighbourWannabe);
    }
    return result;
}

