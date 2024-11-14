#ifndef COMPILER_PASS_H
#define COMPILER_PASS_H

#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/raw_ostream.h"

#include <sstream>
#include <string>
#include <typeinfo>
#include <iostream>
#include <fstream>

using namespace clang;
using namespace clang::driver;
using namespace clang::tooling;

static llvm::cl::OptionCategory ToolingSampleCategory("Tooling Sample");

/**
 * Run a compiler pass to modify code
 * template T: the class name of the ASTVisitor used to modify the code
 * \param sourceCode: the source code to be modified
 * \return the modified code after a compiler pass
 */
template <typename T> std::string compilerPass(const std::string& sourceCode);

/**
 * Run a compiler pass to modify code
 * template T: the class name of the ASTVisitor used to modify the code
 * \param argc: the number of arguments passed
 * \param argv: the array of character pointers listing all args
 * \return The modified code after a compiler pass
 */
template <typename T> std::string compilerPass(int argc, const char **argv);

#endif