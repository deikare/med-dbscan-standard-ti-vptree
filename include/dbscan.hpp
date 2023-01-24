#include "consts.hpp"
#include <map>
#include <set>
#include <vector>
#include <functional>
#include <string>

#define NOISE (-1)

using DataPoint = std::vector<double>;

class DBScan {
private:
    std::map<DataPoint, long> clusterizeResult;
    const unsigned long minPts;

    std::set<DataPoint> neighbours(const DataPoint &point, const std::vector<DataPoint> &points,
                                   const std::function<double(DataPoint, DataPoint)> &distanceHandler);


protected:
    DBScan(unsigned long minPts, double eps);

    void
    addToResultIfNeighbour(const DataPoint &point, const DataPoint &potentialNeighbour, std::set<DataPoint> &result,
                           const std::function<double(DataPoint, DataPoint)> &distanceHandler) const;

    const double eps;

    void performClustering(const std::vector<DataPoint> &points,
                           const std::function<std::set<DataPoint>(DataPoint)> &neighboursHandler);

public:
    DBScan(const std::vector<DataPoint> &points, const std::function<double(DataPoint, DataPoint)> &distanceHandler,
           unsigned int minPts, double eps);


    void printResultToFile(const std::string &filename);
};

class DBScanTi : public DBScan {
public:
    DBScanTi(const std::vector<DataPoint> &points, const std::function<double(DataPoint, DataPoint)> &distanceHandler,
             double eps, unsigned int minPts, DataPoint refPoint);

private:
    const DataPoint refPoint;

    struct dontCompare { //disable compare of keys in TI map
        bool operator()(const DataPoint &a, const DataPoint &b) const {
            return (a < b) ? true : true; //dummy code so clang dont return errors
        }
    };

    std::map<DataPoint, double, dontCompare> generateReferenceTable(const std::vector<DataPoint> &points,
                                                                    const std::function<double(DataPoint,
                                                                                               DataPoint)> &distanceHandler);

    std::set<DataPoint> neighboursTI(const DataPoint &point, std::map<DataPoint, double, dontCompare> referenceTable,
                                     const std::function<double(const DataPoint &,
                                                                const DataPoint &)> &distanceHandler);
};