#include <fstream>
#include <iostream>

#include "analyzer.hpp"
#include "parser.hpp"
#include "reader.hpp"
#include "vp_tree.hpp"
#include "dbscan.hpp"
#include "math.hpp"

bool check_file(std::string file_name) {
  std::ifstream input_stream;
  input_stream.open(file_name.c_str(), std::ios::in);
  if (!input_stream) {
    return false;
  }
  return true;
}

int main(int argc, char* argv[]) {
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
  int ignore_lines;
  if (parser.cmdOptionExists("-i"))
    ignore_lines = stoi(parser.getCmdOption("-i"));

  auto csv_reader = CSVReader();
  auto [data, classes] = csv_reader.getData(filename, true, ignore_lines);
  time_analyzer.saveDurationNow("reading csv");

  VPTree vp_tree(data);
  time_analyzer.saveDurationNow("building VP tree");

  auto vec = vp_tree.findNeighbours(data, 3, 2.);
  time_analyzer.saveDurationNow("searching k-NN+ in VP tree");

  auto distanceHandler = [](const std::vector<double>& point1, const std::vector<double>&point2) {
      return math::minkowskiDist(point1, point2, 2);
  };

  double eps = 1; //try 1, 3
  unsigned int minPts = 5;
  std::vector<double> refPoint = {0, 0};
  std::vector<ReferencePointType> references = {MIN, MAX};


//    DBScan result = DBScan(data, distanceHandler, minPts, eps);
  DBScanTi result = DBScanTi(data, distanceHandler, eps, minPts, refPoint);
//  DBScanTi result = DBScanTi(data, distanceHandler, eps, minPts, MAX);
//  DBScanTi result = DBScanTi(data, distanceHandler, eps, minPts, MIN);
//  DBScanTi result = DBScanTi(data, distanceHandler, eps, minPts, references);
  result.printResultToFile("../data/dbscan-result-new.csv");
  result.generateOutFile("../data", "Dane");
  result.generateStatFile("../data", "Dane");

  time_analyzer.printRaport();

  return 0;
}