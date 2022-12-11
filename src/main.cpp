#include "parser.hpp"

int main(int argc, char* argv[]) {
  auto parser = Parser(argc, argv);
  if (parser.cmdOptionExists("-h")) parser.printHelp();
  if (!parser.allArgsSet()) return 1;

  return 0;
}