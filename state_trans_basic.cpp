#include "clang/AST/ASTContext.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Tooling/Tooling.h"

#include <sstream>
#include <iostream>
#include <fstream>

using namespace clang;

class StateDependencyVisitor : public RecursiveASTVisitor<StateDependencyVisitor> {
  public:
    explicit StateDependencyVisitor(ASTContext *Context, Rewriter &R) : Context(Context), TheRewriter(R) {}

    bool VisitBinaryOperator(BinaryOperator *op) {
      if (op->getOpcode() == BO_Assign || op->isCompoundAssignmentOp()) {
        if (DeclRefExpr *lhs = dyn_cast<DeclRefExpr>(op->getLHS())) {
          if (lhs->getNameInfo().getAsString() == "state") {
            FullSourceLoc loc = Context->getFullLoc(op->getExprLoc());
            if (loc.isValid()) {
              llvm::outs() << "Found modification to 'state' at line " << loc.getSpellingLineNumber() << "\n";
            }
            rewriteStateModification(TheRewriter, op);
          }
        }
      }
      return true;
    }

    bool VisitUnaryOperator(UnaryOperator *op) {
      if (op->isIncrementDecrementOp()) {
        if (DeclRefExpr *operand = dyn_cast<DeclRefExpr>(op->getSubExpr())) {
          if (operand->getNameInfo().getAsString() == "state") {
            FullSourceLoc loc = Context->getFullLoc(op->getExprLoc());
            if (loc.isValid()) {
              llvm::outs() << "Found modification to 'state' at line " << loc.getSpellingLineNumber() << "\n";
            }
            rewriteStateModification(TheRewriter, op);
          }
        }
      }
      return true;
    }
  
  private:
    ASTContext *Context;
    Rewriter &TheRewriter;

    void rewriteStateModification(clang::Rewriter &TheRewriter, clang::Stmt *StateModifyingStmt) {
      // get the source range of the modifying statement
      clang::SourceRange range = StateModifyingStmt->getSourceRange();

      // retrieve the original text
      std::string originalText = TheRewriter.getRewrittenText(range);
      
      // make the transformation
      std::string transformedText = "for (int i = 0; i < NUM_PKTS - 1); i++) {\n" + originalText + ";\n}\n" + originalText + ";";

      TheRewriter.ReplaceText(range, transformedText);
    }
};

class StateDependencyConsumer : public clang::ASTConsumer {
  public:
    explicit StateDependencyConsumer(ASTContext *Context, Rewriter &R)
      : TheRewriter(R), Context(Context), Visitor(Context, TheRewriter) {}

    virtual void HandleTranslationUnit(clang::ASTContext &C) override {
      Visitor.TraverseDecl(C.getTranslationUnitDecl());
    }

  private:
    StateDependencyVisitor Visitor;
    ASTContext *Context;
    Rewriter &TheRewriter;
};

class StateDependencyAction : public clang::ASTFrontendAction {
  public:
    StateDependencyAction() {}

    void EndSourceFileAction() override {
      TheRewriter.getEditBuffer(TheRewriter.getSourceMgr().getMainFileID()).write(llvm::outs());
    }

    virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance &CI, llvm::StringRef file) override {
      TheRewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
      return std::make_unique<StateDependencyConsumer>(&CI.getASTContext(), TheRewriter);
    }
  
  private:
    Rewriter TheRewriter;
};

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

  std::stringstream buf;
  buf << file.rdbuf();
  std::string code = buf.str();

  // run on input
  if (argc > 1) {
    clang::tooling::runToolOnCode(std::make_unique<StateDependencyAction>(), code);
  }

  return 0;
}
