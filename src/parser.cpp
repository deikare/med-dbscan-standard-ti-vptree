#include "parser.hpp"

#include <algorithm>
#include <iostream>

std::string Parser::getCmdOption(const std::string& option) {
  auto end = argv_ + argc_;
  char** itr = std::find(argv_, end, option);
  if (itr != end && ++itr != end) {
    return std::string(*itr);
  }
  return 0;
}

bool Parser::cmdOptionExists(const std::string& option) {
  auto start = argv_;
  auto end = argv_ + argc_;
  return std::find(start, end, option) != (end);
}

bool Parser::allArgsSet() {
  if (!cmdOptionExists("-f")) {
    std::cout << "No data file provided \n";
    return false;
  }
  if (!cmdOptionExists("-i")) {
    std::cout << "No ignore lines parameter was given \n";
    return false;
  }
  return true;
}

void Parser::printHelp() {
  std::cout << "Program options: \n";
  std::cout << "-h get help \n\n";

  std::cout << "Obligatory: \n";
  std::cout << "-f provide filename \n";
  std::cout << "-i ignore n initial lines of file \n\n";

  std::cout << "Optional: \n";
  std::cout << "-a [std|ti|vp] - algorithm version, default = std\n";
  std::cout << "-e epsilon, default = 1 \n";
  std::cout << "-m minPts, default = 2 \n";
  std::cout << "-rv [min|max|point=0,0,...,0|refOpts=min,max,...,max] - specify reference point type for TI, default = max \n";
}
