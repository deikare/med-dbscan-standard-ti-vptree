#pragma once
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

class CSVReader {
  std::vector<double> stripLine_(std::string line);

 public:
  std::pair<std::vector<std::vector<double>>, std::vector<long>> getData(
      std::string filename, bool has_class_column, int skip_lines);
};
