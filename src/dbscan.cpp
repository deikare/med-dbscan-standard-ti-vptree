//
// Created by deikare on 05.01.23.
//
#include <fstream>
#include <utility>
#include "dbscan.hpp"

void DBScan::performClustering(const std::vector<DataPoint> &points,
                               const std::function<std::set<DataPoint>(DataPoint)> &neighboursHandler) {
    long clusterIndex = NOISE;

    for (const auto &point: points) {
        if (clusterizeResult.find(point) == clusterizeResult.end()) {
            auto seeds = neighboursHandler(point);
            if (seeds.size() < minPts) {
                clusterizeResult.emplace(point, NOISE);
                continue;
            }

            clusterIndex++;
            clusterizeResult.emplace(point, clusterIndex);
            seeds.erase(point);

            for (const auto &seed: seeds) {
                auto entry = clusterizeResult.find(seed);
                if (entry == clusterizeResult.end()) { //seed is unclassified
                    clusterizeResult.emplace(seed, clusterIndex);
                    auto seedNeighbours = neighboursHandler(seed);
                    if (seedNeighbours.size() >= minPts) //seed is core
                        seeds.merge(seedNeighbours);
                } else if (entry->second == NOISE)
                    entry->second = clusterIndex;
            }
        }
    }
}

DBScan::DBScan(const std::vector<DataPoint> &points, const std::function<double(DataPoint, DataPoint)> &distanceHandler,
               unsigned int minPts, double eps) : minPts(minPts), eps(eps) {
    initializePointStatistics(points);

    auto neighboursHandler = [this, distanceHandler, points](const DataPoint &point) {
        return this->neighbours(point, points, distanceHandler);
    };


    performClustering(points, neighboursHandler);
}

std::set<DataPoint> DBScan::neighbours(const DataPoint &point, const std::vector<DataPoint> &points,
                                       const std::function<double(DataPoint, DataPoint)> &distanceHandler) {
    std::set<DataPoint> result;
    for (const auto &potentialNeighbour: points) {
        addToResultIfNeighbour(point, potentialNeighbour, result, distanceHandler);
    }
    return result;
}

void
DBScan::addToResultIfNeighbour(const DataPoint &point, const DataPoint &potentialNeighbour, std::set<DataPoint> &result,
                               const std::function<double(DataPoint, DataPoint)> &distanceHandler) const {
    if (distanceHandler(point, potentialNeighbour) <= eps)
        result.emplace(potentialNeighbour);
}

void DBScan::printResultToFile(const std::string &filename) {
    std::string result = std::to_string(eps) + "\n";

    for (const auto &entry: clusterizeResult) {
        for (auto attribute: entry.first)
            result += std::to_string(attribute) + ",";
        result += std::to_string(entry.second) + "\n";
    }

    std::ofstream file;
    file.open(filename);
    file << result;
    file.close();
}

void DBScan::initializePointStatistics(const std::vector<DataPoint> &points) {
    PointStatistics::NEXT_POINT_ID = 0;
    for (const auto &point: points)
        pointStatistics.emplace(point, PointStatistics());
}

DBScan::DBScan(const std::vector<DataPoint> &points, unsigned long minPts, double eps) : minPts(minPts), eps(eps) {
    initializePointStatistics(points);
}


DBScanTi::DBScanTi(const std::vector<DataPoint> &points,
                   const std::function<double(DataPoint, DataPoint)> &distanceHandler,
                   double eps, unsigned int minPts, DataPoint refPoint)
        : DBScan(points, minPts, eps), refPoint(std::move(refPoint)) {

    auto table = generateReferenceTable(points, distanceHandler);

    auto neighboursHandler = [this, distanceHandler, table](const DataPoint &point) {
        return this->neighboursTI(point, table, distanceHandler);
    };

    performClustering(points, neighboursHandler);
}

std::map<DataPoint, double, DBScanTi::dontCompare>
DBScanTi::generateReferenceTable(const std::vector<DataPoint> &points,
                                 const std::function<double(DataPoint,
                                                            DataPoint)> &distanceHandler) {
    std::vector<std::pair<DataPoint, double>> resultAsVector;
    for (const auto &point: points)
        resultAsVector.emplace_back(point, distanceHandler(point, refPoint));

    std::sort(resultAsVector.begin(), resultAsVector.end(),
              [](const std::pair<DataPoint, double> &a, const std::pair<DataPoint, double> &b) {
                  return a.second > b.second;
              });

    std::map<DataPoint, double, dontCompare> result;
    for (const auto &pair: resultAsVector)
        result.emplace(pair.first, pair.second);
    return result;
}

std::set<DataPoint>
DBScanTi::neighboursTI(const DataPoint &point, std::map<DataPoint, double, DBScanTi::dontCompare> referenceTable,
                       const std::function<double(const DataPoint &, const DataPoint &)> &distanceHandler) {
    std::set<DataPoint> result = {point};

    auto iterator = referenceTable.find(point);
    auto referencedDistance = iterator->second;

    auto rend = referenceTable.rend();
    std::map<DataPoint, double, DBScanTi::dontCompare>::reverse_iterator reverseIterator(iterator);
    for (reverseIterator++; reverseIterator != rend; reverseIterator++) {
        if (referencedDistance - reverseIterator->second <= eps) {
            addToResultIfNeighbour(point, reverseIterator->first, result, distanceHandler);
        } else break;
    }

    auto end = referenceTable.end();
    for (iterator++; iterator != end; iterator++) {
        if (iterator->second - referencedDistance <= eps) {
            addToResultIfNeighbour(point, iterator->first, result, distanceHandler);
        } else break;
    }

    return result;
}

DBScan::PointStatistics::PointStatistics() : id(NEXT_POINT_ID++) {

}
