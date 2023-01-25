#include "consts.hpp"
#include <map>
#include <set>
#include <vector>
#include <functional>
#include <string>
#include "analyzer.hpp"

#define NOISE (-1)
#define BORDER 0
#define CORE 1

using DataPoint = std::vector<double>;

class DBScan {
private:
    std::map<DataPoint, long> clusterizeResult;
    const unsigned long minPts;
    unsigned long clusterCount;


    std::set<DataPoint> neighbours(const DataPoint &point, const std::vector<DataPoint> &points,
                                   const std::function<double(DataPoint, DataPoint)> &distanceHandler);

    void initializePointStatistics(const std::vector<DataPoint> &points);

    class PointStatistics {
    public:
        inline static unsigned long NEXT_POINT_ID = 0;
        const unsigned long id;
        unsigned long distanceCalculationCount = 0;
        int pointType;
        std::set<unsigned long> neighbourhood;

        explicit PointStatistics();

        std::string produceOutLine(const DataPoint &point, long clusterId);
    };

    void setNeighbourhood(const DataPoint &point, const std::set<DataPoint> &neighbourhood);

    void setPointType(const DataPoint &point, int type);

    static void writeToFile(const std::string &string, const std::string &filename);

protected:
    DBScan(const std::vector<DataPoint> &points, unsigned long minPts, double eps);

    void
    addToResultIfNeighbour(const DataPoint &point, const DataPoint &potentialNeighbour, std::set<DataPoint> &result,
                           const std::function<double(DataPoint, DataPoint)> &distanceHandler) const;

    void addToDistanceCalculationCount(const DataPoint &point, unsigned long increment);

    std::string generateFilename(const std::string &prefix, const std::string &type, const std::string &datafileName,
                                 const std::string &algorithmVersion);

    const double eps;

    void performClustering(const std::vector<DataPoint> &points,
                           const std::function<std::set<DataPoint>(DataPoint)> &neighboursHandler);

    void
    generateOutFile(const std::string &prefix, const std::string &datafileName, const std::string &algorithmVersion);

    void
    generateStatFile(const std::string &prefix, const std::string &datafileName, const std::string &algorithmVersion);

    std::string produceStatFileContents(const std::string &datafileName);

    std::map<DataPoint, PointStatistics> pointStatistics;

    TimeAnalyzer timeAnalyzer = TimeAnalyzer(TIME_PRESCALER::MICRO);
    const std::string NEIGHBOURHOOD_CALCULATION = "neighbourhood-calculation";
    const std::string CLUSTERING = "clustering";
    const std::string OUTFILE_CREATION = "outfile-creation";
    const std::string TOTAL_TASK = "total-task";


    std::string suffix;
public:
    DBScan(const std::vector<DataPoint> &points, const std::function<double(DataPoint, DataPoint)> &distanceHandler,
           unsigned int minPts, double eps);

    void printResultToFile(const std::string &filename);

    virtual void generateOutFile(const std::string &prefix, const std::string &datafileName);

    virtual void generateStatFile(const std::string &prefix, const std::string &datafileName);
};

enum ReferencePointType {
    MAX, MIN
};

class DBScanTi : public DBScan {
public:
    DBScanTi(const std::vector<DataPoint> &points, const std::function<double(DataPoint, DataPoint)> &distanceHandler,
             double eps, unsigned int minPts, enum ReferencePointType referencePointType);
    DBScanTi(const std::vector<DataPoint> &points, const std::function<double(DataPoint, DataPoint)> &distanceHandler,
             double eps, unsigned int minPts, const std::vector<ReferencePointType>& referencePointType);
    DBScanTi(const std::vector<DataPoint> &points, const std::function<double(DataPoint, DataPoint)> &distanceHandler,
             double eps, unsigned int minPts, const DataPoint& refPoint);

    void generateOutFile(const std::string &prefix, const std::string &datafileName) override;

    void generateStatFile(const std::string &prefix, const std::string &datafileName) override;

private:
    DataPoint refPoint;

    void calculateRefPoint(const std::vector<DataPoint> &points, enum ReferencePointType referencePointType);

    void calculateRefPoint(const std::vector<DataPoint> &points, const std::vector<ReferencePointType>& attributeRefs);

    void calculateRefPointAttribute(const std::vector<DataPoint> &points, enum ReferencePointType referencePointType, unsigned long attribute);

    class ReferenceTable {
    private:
        std::map<DataPoint, long> indexMap;
        std::vector<std::pair<DataPoint, double>> distancesToReference;
    public:
        ReferenceTable(const std::vector<DataPoint> &points,
                       const std::function<double(DataPoint,
                                                  DataPoint)> &distanceHandler, const DataPoint &refPoint);

        [[nodiscard]] std::vector<std::pair<DataPoint, double>>::const_iterator iterator(const DataPoint &point) const;

        [[nodiscard]] std::vector<std::pair<DataPoint, double>>::const_reverse_iterator
        reverseIterator(const DataPoint &point) const;

        [[nodiscard]] std::vector<std::pair<DataPoint, double>>::const_iterator end() const;

        [[nodiscard]] std::vector<std::pair<DataPoint, double>>::const_reverse_iterator rend() const;

    };

    std::set<DataPoint> neighboursTI(const DataPoint &point,
                                     const std::function<double(const DataPoint &,
                                                                const DataPoint &)> &distanceHandler,
                                     const ReferenceTable &referenceTable);
};