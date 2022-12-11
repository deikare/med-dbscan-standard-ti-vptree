#pragma once
#include <string>

class Parser {
  int argc_;
  char** argv_;

 public:
  Parser(int argc, char* argv[]) {
    this->argc_ = argc;
    this->argv_ = argv;
  }
  std::string getCmdOption(const std::string& option);
  bool cmdOptionExists(const std::string& option);
  bool allArgsSet();
  void printHelp();
};
