#include "compilerPass.h"

template<typename T> class GenericASTConsumer : public ASTConsumer {
public:
  GenericASTConsumer(Rewriter &R) : Visitor(R) {}

  
}


template<typename T> std::string compilerPass(const std::string& sourceCode) {

}

template<typename T> std::string compilerPass(int argc, const char **argv) {

}