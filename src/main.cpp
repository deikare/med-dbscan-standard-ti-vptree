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

  double eps = 1;
  unsigned int minPts = 5;
  auto dbscanResult = dbscan(data, distanceHandler, eps, minPts);
  std::string result = std::to_string(eps) + "\n";

  for (const auto& entry : dbscanResult) {
      for (auto attribute : entry.first)
          result += std::to_string(attribute) + ",";
      result += std::to_string(entry.second) + "\n";
  }

  std::cout << result << std::endl;

  std::ofstream file;
  file.open("../data/dbscan-result.csv");
  file << result;
  file.close();

  time_analyzer.printRaport();

  return 0;
}