#include <fstream>
#include <iostream>

#include "parser.hpp"
#include "reader.hpp"
#include "vp_tree.hpp"

bool check_file(std::string file_name) {
  std::ifstream input_stream;
  input_stream.open(file_name.c_str(), std::ios::in);
  if (!input_stream) {
    return false;
  }
  return true;
}

int main(int argc, char* argv[]) {
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
  VPTree tree;
  tree.createTree(data);

  return 0;
}