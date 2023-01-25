#include "consts.hpp"
#include <map>
#include <set>
#include <vector>
#include <functional>
#include <string>

#define NOISE (-1)
#define BORDER 0
#define CORE 1

using DataPoint = std::vector<double>;

class DBScan {
private:
    std::map<DataPoint, long> clusterizeResult;
    const unsigned long minPts;

    std::set<DataPoint> neighbours(const DataPoint &point, const std::vector<DataPoint> &points,
                                   const std::function<double(DataPoint, DataPoint)> &distanceHandler);

    void initializePointStatistics(const std::vector<DataPoint> &points);

    class PointStatistics {
    public:
        inline static unsigned long NEXT_POINT_ID = 0;
        const unsigned long id;
        unsigned long distanceCalculationCount = 0;
        int8_t pointType = NOISE; //default
        std::set<unsigned long> neighbourhood;

        explicit PointStatistics();

        std::string produceOutLine(const DataPoint& point, long clusterId);
    };

    std::map<DataPoint, PointStatistics> pointStatistics;

    void addToDistanceCalculationCount(const DataPoint& point, unsigned long increment);
    void setNeighbourhood(const DataPoint& point, const std::set<DataPoint>& neighbourhood);
    void setPointType(const DataPoint& point, int8_t type);

    static void writeToFile(const std::string& string, const std::string &filename);

protected:
    DBScan(const std::vector<DataPoint> &points, unsigned long minPts, double eps);

    void
    addToResultIfNeighbour(const DataPoint &point, const DataPoint &potentialNeighbour, std::set<DataPoint> &result,
                           const std::function<double(DataPoint, DataPoint)> &distanceHandler) const;

    const double eps;

    void performClustering(const std::vector<DataPoint> &points,
                           const std::function<std::set<DataPoint>(DataPoint)> &neighboursHandler);
    void generateOutFile(const std::string& datafileName, const std::string& algorithmVersion);

public:
    DBScan(const std::vector<DataPoint> &points, const std::function<double(DataPoint, DataPoint)> &distanceHandler,
           unsigned int minPts, double eps);

    void printResultToFile(const std::string &filename);

    void generateOutFile(const std::string& datafileName);
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