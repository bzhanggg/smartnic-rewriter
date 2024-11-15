#pragma once

#include "clang/AST/ASTContext.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Tooling/Tooling.h"

using namespace clang;

class ChangeStateVisitor : public RecursiveASTVisitor<ChangeStateVisitor> {
public:
  explicit ChangeStateVisitor(Rewriter &R) : TheRewriter(R) {}
  bool VisitBinaryOperator(BinaryOperator *op);
  bool VisitUnaryOperator(UnaryOperator *op);

private:
  Rewriter &TheRewriter;
  void rewriteStateChange(clang::Stmt *StateChangingStmt);
};
