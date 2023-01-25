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
            setNeighbourhood(point, seeds);
            if (seeds.size() < minPts) {
                clusterizeResult.emplace(point, NOISE);
                continue;
            }

            clusterIndex++;
            clusterizeResult.emplace(point, clusterIndex);
            setPointType(point, CORE);
            seeds.erase(point);

            for (const auto &seed: seeds) {
                auto entry = clusterizeResult.find(seed);
                if (entry == clusterizeResult.end()) { //seed is unclassified
                    clusterizeResult.emplace(seed, clusterIndex);
                    auto seedNeighbours = neighboursHandler(seed);
                    setNeighbourhood(seed, seedNeighbours);
                    if (seedNeighbours.size() >= minPts) {
                        setPointType(seed, CORE);
                        seeds.merge(seedNeighbours);
                    } //seed is core
                } else if (entry->second == NOISE) {
                    entry->second = clusterIndex;
                    setPointType(seed, BORDER);
                }
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
    pointStatistics.find(point)->second.pointType = BORDER;
    for (const auto &potentialNeighbour: points) {
        addToResultIfNeighbour(point, potentialNeighbour, result, distanceHandler);
    }
    addToDistanceCalculationCount(point, points.size());
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

    writeToFile(result, filename);
}

void DBScan::initializePointStatistics(const std::vector<DataPoint> &points) {
    for (const auto &point: points)
        pointStatistics.emplace(point, PointStatistics());
}

DBScan::DBScan(const std::vector<DataPoint> &points, unsigned long minPts, double eps) : minPts(minPts), eps(eps) {
    initializePointStatistics(points);
}

void DBScan::addToDistanceCalculationCount(const DataPoint &point, unsigned long increment) {
    auto entry = pointStatistics.find(point);
    entry->second.distanceCalculationCount += increment;
}

void DBScan::setNeighbourhood(const DataPoint &point, const std::set<DataPoint> &neighbourhood) {
    auto entry = pointStatistics.find(point);
    if (entry->second.neighbourhood.empty()) {
        for (const auto &neighbour: neighbourhood) {
            auto neighbourEntry = pointStatistics.find(neighbour);
            entry->second.neighbourhood.emplace(neighbourEntry->second.id);
        }
    }
}

void DBScan::setPointType(const DataPoint &point, int8_t type) {
    auto entry = pointStatistics.find(point);
    entry->second.pointType = type;
}

void DBScan::generateOutFile(const std::string &prefix, const std::string &datafileName) {
    generateOutFile(prefix, datafileName, "DBSCAN");

}

void DBScan::writeToFile(const std::string &string, const std::string &filename) {
    std::ofstream file;
    file.open(filename);
    file << string;
    file.close();
}

void DBScan::generateOutFile(const std::string &prefix, const std::string &datafileName,
                             const std::string &algorithmVersion) {
    std::string result;

    for (auto &iterator: clusterizeResult) {
        DataPoint point = iterator.first;
        long clusterIndex = iterator.second;

        result += pointStatistics.find(point)->second.produceOutLine(point, clusterIndex) + "\n";
    }

    std::string outFileName = (prefix.empty() ? "" : (prefix + "/"));
    outFileName += generateFilename(prefix, "out", datafileName, algorithmVersion);

    writeToFile(result, outFileName);
}

std::string
DBScan::generateFilename(const std::string& prefix, const std::string& type, const std::string& datafileName, const std::string& algorithmVersion) {
    std::string result = (prefix.empty() ? "" : (prefix + "/"));
    result += type + "_" + algorithmVersion + "_" + datafileName + "_D" +
              std::to_string(clusterizeResult.begin()->first.size()) + "_R" +
              std::to_string(clusterizeResult.size()) + "_mP" + std::to_string(minPts) + "_E" +
              std::to_string(eps) + ".txt";

    return result;
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

    unsigned long distanceCalculationCount = 0;

    auto rend = referenceTable.rend();
    std::map<DataPoint, double, DBScanTi::dontCompare>::reverse_iterator reverseIterator(iterator);
    for (reverseIterator++; reverseIterator != rend; reverseIterator++) {
        if (referencedDistance - reverseIterator->second <= eps) {
            addToResultIfNeighbour(point, reverseIterator->first, result, distanceHandler);
            distanceCalculationCount++;
        } else break;
    }

    auto end = referenceTable.end();
    for (iterator++; iterator != end; iterator++) {
        if (iterator->second - referencedDistance <= eps) {
            addToResultIfNeighbour(point, iterator->first, result, distanceHandler);
            distanceCalculationCount++;
        } else break;
    }

    addToDistanceCalculationCount(point, distanceCalculationCount);

    return result;
}

void DBScanTi::generateOutFile(const std::string &prefix, const std::string &datafileName) {
    DBScan::generateOutFile(prefix, datafileName, "TI-DBSCAN");
}

DBScan::PointStatistics::PointStatistics() : id(NEXT_POINT_ID++) {

}

std::string DBScan::PointStatistics::produceOutLine(const DataPoint &point, long clusterId) {
    std::string result;
    result += std::to_string(id) + ",";
    for (auto attribute: point)
        result += std::to_string(attribute) + ",";

    result += std::to_string(distanceCalculationCount) + ",";
    result += std::to_string(pointType) + ",";
    result += std::to_string(clusterId) + ",";

    for (auto neighbour: neighbourhood)
        result += std::to_string(neighbour) + ",";

    result += std::to_string(neighbourhood.size());

    return result;
}



