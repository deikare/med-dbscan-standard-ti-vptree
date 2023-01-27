#include <fstream>
#include <iostream>

#include "analyzer.hpp"
#include "parser.hpp"
#include "reader.hpp"
#include "vp_tree.hpp"
#include "dbscan.hpp"
#include "dbscan_vp_tree.h"
#include "math.hpp"

bool check_file(std::string file_name) {
    std::ifstream input_stream;
    input_stream.open(file_name.c_str(), std::ios::in);
    if (!input_stream) {
        return false;
    }
    return true;
}

int main(int argc, char *argv[]) {
    auto time_analyzer = TimeAnalyzer(TIME_PRESCALER::MICRO);
    auto parser = Parser(argc, argv);
    if (parser.cmdOptionExists("-h")) {
        parser.printHelp();
        return 0;
    }
    if (!parser.allArgsSet()) return 1;

    auto filename = parser.getCmdOption("-f");
    if (!check_file(filename)) {
        std::cout << "File " << filename << " cannot be opened \n";
        return 1;
    }
    int ignore_lines = -1;
    if (parser.cmdOptionExists("-i"))
        ignore_lines = stoi(parser.getCmdOption("-i"));

    std::string algorithmVersion = "std";
    if (parser.cmdOptionExists("-a"))
        algorithmVersion = parser.getCmdOption("-a");

    auto csv_reader = CSVReader();
    auto [data, classes] = csv_reader.getData(filename, true, ignore_lines);
    time_analyzer.saveDurationNow("reading csv");


    double eps = 1; //try 1, 3
    if (parser.cmdOptionExists("-e"))
        eps = stod(parser.getCmdOption("-e"));

    unsigned int minPts = 2;
    if (parser.cmdOptionExists("-m"))
        minPts = stoi(parser.getCmdOption("-m"));

    std::string baseFilename = filename.substr(filename.find_last_of("/\\") + 1);
    std::string::size_type const p(baseFilename.find_last_of('.'));
    std::string baseFilenameNoExtension = baseFilename.substr(0, p);

    if (algorithmVersion == "std") {
        auto distanceHandler = [](const std::vector<double>& point1, const std::vector<double>&point2) {
            return math::minkowskiDist(point1, point2, 2);
        };

        DBScan result = DBScan(data, distanceHandler, minPts, eps);
        result.printResultToFile("../data/dbscan-result-new.csv");
        result.generateOutFile("../data", baseFilenameNoExtension);
        result.generateStatFile("../data", baseFilenameNoExtension, classes);
    }

    else if (algorithmVersion == "ti") {
        auto distanceHandler = [](const std::vector<double>& point1, const std::vector<double>&point2) {
            return math::minkowskiDist(point1, point2, 2);
        };

        std::string refPointVersion = "max";
        if (parser.cmdOptionExists("-rv"))
            refPointVersion = parser.getCmdOption("-rv");

        DBScanTi* result;

        if (refPointVersion == "max")
            result = new DBScanTi(data, distanceHandler, eps, minPts, MAX);

        else if (refPointVersion == "min")
            result = new DBScanTi(data, distanceHandler, eps, minPts, MIN);

        else {
            std::vector<std::string> refArgs;
            std::stringstream stream(refPointVersion);
            std::string segment;
            while (std::getline(stream, segment, '=' ))
                refArgs.emplace_back(segment);

            if (refArgs.size() != 2) {
                std::cout << "Wrong refArg format";
                exit(-4);
            }

            if (refArgs[0] == "point") {
                std::vector<double> refPoint;
                std::stringstream attributesStream(refArgs[1]);
                while (std::getline(attributesStream, segment, ','))
                    refPoint.emplace_back(stod(segment));

                result = new DBScanTi(data, distanceHandler, eps, minPts, refPoint);
            }

            else if (refArgs[0] == "refOpts") {
                std::vector<ReferencePointType> references;
                std::stringstream referenceStream(refArgs[1]);
                while (std::getline(referenceStream, segment, ',')) {
                    if (segment == "min")
                        references.emplace_back(MIN);
                    else if (segment == "max")
                        references.emplace_back(MAX);
                    else {
                        std::cout << "Wrong references option format";
                        exit(-4);
                    }
                }

                result = new DBScanTi(data, distanceHandler, eps, minPts, references);
            }
            else {
                std::cout << "Wrong ref option";
                exit(-4);
            }
        }

        result->printResultToFile("../data/dbscan-result-new.csv");
        result->generateOutFile("../data", baseFilenameNoExtension);
        result->generateStatFile("../data", baseFilenameNoExtension, classes);
    }

    else if (algorithmVersion == "vp") {
        DBScanVPTree result = DBScanVPTree(data, minPts, eps);

        result.printResultToFile("../data/dbscan-result-new.csv");
        result.generateOutFile("../data", baseFilenameNoExtension);
        result.generateStatFile("../data", baseFilenameNoExtension, classes);
    }

    time_analyzer.printRaport();

    return 0;
}