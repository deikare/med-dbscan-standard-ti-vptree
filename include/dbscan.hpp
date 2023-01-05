#include "consts.hpp"
#include <map>
#include <set>
#include <vector>
#include <functional>

#define NOISE 0

using DataPoint = std::vector<double>;

std::map<DataPoint, unsigned long> dbscan(const std::vector<DataPoint>& points,const std::function<double(DataPoint, DataPoint)>& distanceHandler,double eps,unsigned minPts);

std::set<DataPoint> neighbours(const DataPoint & point, const std::vector<DataPoint> & points, const std::function<double (const DataPoint&, const DataPoint&)>& distanceHandler, double eps);