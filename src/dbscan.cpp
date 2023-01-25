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
                    } else {
                        setPointType(seed, BORDER);
                    }
                } else if (entry->second == NOISE) {
                    entry->second = clusterIndex;
                    setPointType(seed, BORDER);
                }
            }
        }
    }

    clusterCount = clusterIndex + 1;
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

void DBScan::setPointType(const DataPoint &point, int type) {
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

    std::string outFileName = generateFilename(prefix, "out", datafileName, algorithmVersion);

    writeToFile(result, outFileName);
}

std::string
DBScan::generateFilename(const std::string &prefix, const std::string &type, const std::string &datafileName,
                         const std::string &algorithmVersion) {
    std::string result = (prefix.empty() ? "" : (prefix + "/"));
    result += type + "_" + algorithmVersion + "_" + datafileName + "_D" +
              std::to_string(clusterizeResult.begin()->first.size()) + "_R" +
              std::to_string(clusterizeResult.size()) + "_mP" + std::to_string(minPts) + "_E" +
              std::to_string(eps) + ".txt";

    return result;
}

void DBScan::generateStatFile(const std::string &prefix, const std::string &datafileName) {
    generateStatFile(prefix, datafileName, "DBSCAN");
}

void DBScan::generateStatFile(const std::string &prefix, const std::string &datafileName,
                              const std::string &algorithmVersion) {
    std::string filename = generateFilename(prefix, "stat", datafileName, algorithmVersion);

    std::string content = produceStatFileContents(datafileName);
    writeToFile(content, filename);
}

std::string DBScan::produceStatFileContents(const std::string &datafileName) {
    std::string result = datafileName + "," + std::to_string(clusterizeResult.begin()->first.size()) + "," +
                         std::to_string(clusterizeResult.size()) + "\n";
    result += std::to_string(minPts) + "," + std::to_string(eps) + "\n";
    unsigned long noiseCount = 0;
    unsigned long borderCount = 0;
    unsigned long coreCount = 0;

    for (const auto &entry: pointStatistics) {
        auto type = entry.second.pointType;
        if (type == NOISE)
            noiseCount++;
        else if (type == BORDER)
            borderCount++;
        else coreCount++;
    }

    result += std::to_string(clusterCount) + "," + std::to_string(noiseCount) + "," + std::to_string(coreCount) + "," +
              std::to_string(borderCount) + "\n";

    unsigned long sum = 0;
    for (const auto &entry: pointStatistics)
        sum += entry.second.distanceCalculationCount;
    double mean = double(sum / pointStatistics.size());

    result += std::to_string(mean) + "\n";
    return result;
}

DBScanTi::DBScanTi(const std::vector<DataPoint> &points,
                   const std::function<double(DataPoint, DataPoint)> &distanceHandler,
                   double eps, unsigned int minPts, const DataPoint &refPoint)
        : DBScan(points, minPts, eps), refPoint(refPoint) {
    auto referenceTable = ReferenceTable(points, distanceHandler, refPoint);
    for (const auto &point: points)
        addToDistanceCalculationCount(point, 1); //add to each because each point is evaluated once
    auto neighboursHandler = [this, referenceTable, distanceHandler](const DataPoint &point) {
        return this->neighboursTI(point, distanceHandler, referenceTable);
    };

    performClustering(points, neighboursHandler);
}

std::set<DataPoint>
DBScanTi::neighboursTI(const DataPoint &point,
                       const std::function<double(const DataPoint &, const DataPoint &)> &distanceHandler,
                       const ReferenceTable &referenceTable) {
    std::set<DataPoint> result = {point};

    unsigned long distanceCalculationCount = 0;

    auto iterator = referenceTable.iterator(point);
    double referencedDistance = iterator->second;

    auto rend = referenceTable.rend();
    auto reverseIterator = referenceTable.reverseIterator(point);
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

void DBScanTi::generateStatFile(const std::string &prefix, const std::string &datafileName) {
    DBScan::generateStatFile(prefix, datafileName, "TI-DBSCAN");
}

DBScan::PointStatistics::PointStatistics() : id(NEXT_POINT_ID++), pointType(NOISE) {

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


DBScanTi::ReferenceTable::ReferenceTable(const std::vector<DataPoint> &points,
                                         const std::function<double(DataPoint, DataPoint)> &distanceHandler,
                                         const DataPoint &refPoint) {
    std::vector<std::pair<DataPoint, double>> resultAsVector;

    for (const auto &point: points)
        resultAsVector.emplace_back(point, distanceHandler(point, refPoint));

    std::sort(resultAsVector.begin(), resultAsVector.end(),
              [](const std::pair<DataPoint, double> &a,
                 const std::pair<DataPoint, double> &b) {
                  return a.second < b.second;
              });

    long i = 0;
    for (const auto &entry: resultAsVector) {
        indexMap.emplace(entry.first, i);
        distancesToReference.emplace_back(entry);
        i++;
    }
}

std::vector<std::pair<DataPoint, double>>::const_iterator
DBScanTi::ReferenceTable::iterator(const DataPoint &point) const {
    auto index = indexMap.find(point)->second;
    return distancesToReference.cbegin() + index;
}

std::vector<std::pair<DataPoint, double>>::const_reverse_iterator
DBScanTi::ReferenceTable::reverseIterator(const DataPoint &point) const {
    auto index = indexMap.find(point)->second;
    return distancesToReference.crbegin() + (indexMap.size() - index - 1);
}

std::vector<std::pair<DataPoint, double>>::const_iterator DBScanTi::ReferenceTable::end() const {
    return distancesToReference.cend();
}

std::vector<std::pair<DataPoint, double>>::const_reverse_iterator DBScanTi::ReferenceTable::rend() const {
    return distancesToReference.crend();
}
