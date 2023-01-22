//
// Created by deikare on 05.01.23.
//
#include "dbscan.hpp"

std::map<DataPoint, long> dbscanImplementation(const std::vector<DataPoint> & points, const std::function<std::set<DataPoint>(DataPoint)> &neighboursHandler, const unsigned int minPts) {
    std::map<DataPoint, long> result;
    long clusterIndex = NOISE;

    for (const auto& point : points) {
        if (result.find(point) == result.end()) {
            auto seeds = neighboursHandler(point);
            if (seeds.size() < minPts) {
                result.emplace(point, NOISE);
                continue;
            }

            clusterIndex++;
            result.emplace(point, clusterIndex);
            seeds.erase(point);
            while (!seeds.empty()) {
                auto seed = *(seeds.begin());

                auto entry = result.find(seed);
                if (entry == result.end()) { //seed is unclassified
                    result.emplace(seed, clusterIndex);
                    auto seedNeighbours = neighboursHandler(point);
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
void addToResultIfNeighbour(const DataPoint& point, const DataPoint& potentialNeighbour, std::set<DataPoint>& result, const std::function<double(const DataPoint&, const DataPoint&)>& distanceHandler, double eps) {
    if (distanceHandler(point, potentialNeighbour) <= eps)
        result.emplace(potentialNeighbour);
}


std::set<DataPoint> neighbours(const DataPoint &point, const std::vector<DataPoint> &points,
                               const std::function<double(const DataPoint&, const DataPoint&)>& distanceHandler, double eps) {
    std::set<DataPoint> result;
    for (const auto& potentialNeighbour : points) {
        addToResultIfNeighbour(point, potentialNeighbour, result, distanceHandler, eps);
    }
    return result;
}

std::map<DataPoint, long> dbscan(const std::vector<DataPoint> & points, const std::function<double(DataPoint, DataPoint)>& distanceHandler, const double eps, const unsigned int minPts) {
    auto neighboursHandler = [eps, distanceHandler, points](const DataPoint& point) {
        return neighbours(point, points, distanceHandler, eps);
    };
    return dbscanImplementation(points, neighboursHandler, minPts);
}


struct dontCompare { //disable compare of keys in TI map
    bool operator()(const DataPoint & a, const DataPoint & b) const {
        return (a < b)? true : true; //dummy code so clang dont return errors
    }
};

std::set<DataPoint> neighboursTI(const DataPoint &point, std::map<DataPoint, double, dontCompare> referenceTable, const std::function<double(const DataPoint&, const DataPoint&)>& distanceHandler, double eps) {
    std::set<DataPoint> result = {point};

    auto rend = referenceTable.rend();
    auto iteratorCopy = referenceTable.find(point);
    auto referencedDistance = iteratorCopy->second;
    for (std::map<DataPoint, double, dontCompare>::reverse_iterator iterator(iteratorCopy); iterator != rend; iterator++) {
        if (referencedDistance - iterator->second <= eps) {
            addToResultIfNeighbour(point, iterator->first, result, distanceHandler, eps);
        }
        else break;
    }

    auto end = referenceTable.end();
    for (auto iterator = iteratorCopy; iterator != end; iterator++) {
        if (iterator->second - referencedDistance <= eps) {
            addToResultIfNeighbour(point, iterator->first, result, distanceHandler, eps);
        }
        else break;
    }

    return result;
}


std::map<DataPoint, double, dontCompare> generateReferenceTable(const std::vector<DataPoint> &points, const DataPoint& refPoint, const std::function<double(DataPoint, DataPoint)> &distanceHandler) {
    std::vector<std::pair<DataPoint, double>> resultAsVector;
    for (const auto& point: points)
        resultAsVector.emplace_back(point, distanceHandler(point, refPoint));

    std::sort(resultAsVector.begin(), resultAsVector.end(), [](const std::pair<DataPoint, double>& a, const std::pair<DataPoint, double>& b) {
        return a.second < b.second;
    });

    std::map<DataPoint, double, dontCompare> result;
    for (const auto& pair: resultAsVector)
        result.emplace(pair.first, pair.second);
    return result;
}

std::map<DataPoint, long> dbscanTI(const std::vector<DataPoint> &points, const std::function<double(DataPoint, DataPoint)> &distanceHandler, double eps, unsigned int minPts, const DataPoint &refPoint) {
    auto table = generateReferenceTable(points, refPoint, distanceHandler);

    auto neighboursHandler = [eps, distanceHandler, table](const DataPoint& point) {
        return neighboursTI(point, table, distanceHandler, eps);
    };

    return dbscanImplementation(points, neighboursHandler, minPts);
}

//implementacja otoczenia TI - przed początkiem dbscana policzyć posortowaną tabelę po odległościach od punktu referencyjnego, i potem stosować nierówność trójkąta
// todo zrobić klasy z mapą idków bindującą do współrzędnych - wtedy będziemy zwracać mapę idków