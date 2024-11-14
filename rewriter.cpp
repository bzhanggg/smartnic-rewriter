#include <sstream>
#include <iostream>
#include <fstream>

int main(int argc, const char **argv) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << "<source-file>\n";
    return EXIT_FAILURE;
  }

  // read input file
  std::ifstream file(argv[1]);
  if (!file) {
    std::cerr << "Error: Could not open file " << argv[1] << "\n";
    return EXIT_FAILURE;
  }
  
  std::string outputFile = "code.out";
  outputFile = argv[1];
  outputFile += ".out";

  ////////////////////////////////////////////////
  // COMPILER PASSES BEGIN HERE
  ////////////////////////////////////////////////
  std::string code = compilerPass<>(argc, argv);

}
