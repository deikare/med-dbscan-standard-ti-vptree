#include "consts.hpp"
#include <map>
#include <set>
#include <vector>
#include <functional>

#define NOISE (-1)

using DataPoint = std::vector<double>;

std::map<DataPoint, long>
dbscan(const std::vector<DataPoint> &points, const std::function<double(DataPoint, DataPoint)> &distanceHandler,
       double eps, unsigned minPts);

std::map<DataPoint, long>
dbscanTI(const std::vector<DataPoint> &points, const std::function<double(DataPoint, DataPoint)> &distanceHandler,
         double eps, unsigned minPts, const DataPoint &refPoint);