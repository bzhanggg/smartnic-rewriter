#include <sstream>
#include <iostream>
#include <fstream>

#include "compilerPass.h"
#include "changeStateVariable.h"

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
  std::string code = compilerPass<ChangeStateVisitor>(argc, argv);
  llvm::errs() << "\nAfter modification, code is modified to \n" << code << "\n";

  std::ofstream fout;
  fout.open(outputFile);
  fout << code;
  fout.close();
  llvm::errs() << "Moficiation ends: " << outputFile << " is the output code." << "\n";
  return 0;
}

