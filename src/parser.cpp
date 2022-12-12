#include <algorithm>
#include <iostream>

#include "parser.hpp"

std::string Parser::getCmdOption(const std::string& option) {
  auto start = argv_;
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
  std::cout << "-h get help \n";
  std::cout << "-f provide filename \n";
  std::cout << "-i ignore n initial lines of file \n";
}
