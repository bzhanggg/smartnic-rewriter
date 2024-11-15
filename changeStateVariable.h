#pragma once

#include "clang/AST/ASTContext.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Tooling/Tooling.h"

#include <sstream>
#include <iostream>
#include <fstream>
#include <streambuf>
#include <iterator>

using namespace clang;

class ChangeStateVisitor : public RecursiveASTVisitor<ChangeStateVisitor> {
public:
  explicit ChangeStateVisitor(Rewriter &R);
  bool VisitBinaryOperator(BinaryOperator *op);
  bool VisitUnaryOperator(UnaryOperator *op);

private:
  Rewriter &TheRewriter;
  void rewriteStateChange(clang::Stmt *StateChangingStmt);
};
