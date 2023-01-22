//
// Created by deikare on 05.01.23.
//
#include "dbscan.hpp"

std::map<DataPoint, long> dbscan(const std::vector<DataPoint> & points, const std::function<double(DataPoint, DataPoint)>& distanceHandler, const double eps, const unsigned int minPts) {
    std::map<DataPoint, long> result;
    long clusterIndex = NOISE;

    for (const auto& point : points) {
        if (result.find(point) == result.end()) {
            auto seeds = neighbours(point, points, distanceHandler, eps);
            if (seeds.size() < minPts) {
                result.emplace(point, NOISE);
                continue;
            }

            clusterIndex++;
            result.emplace(point, clusterIndex);
            seeds.erase(point);
            while (!seeds.empty()) {
                auto seedIterator = seeds.begin();
                auto seed = *(seedIterator);

                auto entry = result.find(seed);
                if (entry == result.end()) { //seed is unclassified
                    result.emplace(seed, clusterIndex);
                    auto seedNeighbours = neighbours(seed, points, distanceHandler, eps);
                    if (seedNeighbours.size() >= minPts) //seed is core
                        seeds.merge(seedNeighbours);
                }
                else if (entry->second == NOISE)
                    entry->second = clusterIndex;

                seeds.erase(seed);
            }
        }
    }

    return result;
}

std::set<DataPoint> neighbours(const DataPoint &point, const std::vector<DataPoint> &points,
                               const std::function<double(const DataPoint&, const DataPoint&)>& distanceHandler, double eps) {
    std::set<DataPoint> result;
    for (const auto& potentialNeighbour : points) {
        if (distanceHandler(potentialNeighbour, point) <= eps)
            result.emplace(potentialNeighbour);
    }
    return result;
}

