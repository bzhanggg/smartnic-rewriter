#include "clang/AST/ASTContext.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Tooling/Tooling.h"

#include <sstream>
#include <iostream>
#include <fstream>

using namespace clang;

class StateModificationVisitor : public RecursiveASTVisitor<StateModificationVisitor> {
  public:
    explicit StateModificationVisitor(Rewriter &R) : TheRewriter(R) {}

    bool VisitBinaryOperator(BinaryOperator *op) {
      if (op->getOpcode() == BO_Assign || op->isCompoundAssignmentOp()) {
        if (DeclRefExpr *lhs = dyn_cast<DeclRefExpr>(op->getLHS())) {
          if (lhs->getNameInfo().getAsString() == "state") {
            rewriteStateModification(op);
          }
        }
      }
      return true;
    }

    bool VisitUnaryOperator(UnaryOperator *op) {
      if (op->isIncrementDecrementOp()) {
        if (DeclRefExpr *operand = dyn_cast<DeclRefExpr>(op->getSubExpr())) {
          if (operand->getNameInfo().getAsString() == "state") {
            rewriteStateModification(op);
          }
        }
      }
      return true;
    }
  
  private:
    Rewriter &TheRewriter;

    void rewriteStateModification(clang::Stmt *StateModifyingStmt) {
      // get the source range of the modifying statement
      clang::SourceRange range = StateModifyingStmt->getSourceRange();
      clang::SourceManager &sourceManager = TheRewriter.getSourceMgr();
      LangOptions langOptions = TheRewriter.getLangOpts();

      CharSourceRange charRange = CharSourceRange::getTokenRange(range);
      std::string originalText = Lexer::getSourceText(charRange, sourceManager, langOptions).str();

      std::string transformedText = "for (int i = 0; i < NUM_PKTS - 1; i++) {\n\t\t" + originalText + ";\n\t}\n\t" + originalText;
      TheRewriter.ReplaceText(range, transformedText);
    }
};

class StateModificationConsumer : public ASTConsumer {
  public:
    explicit StateModificationConsumer(Rewriter &R) : Visitor(R) {}

    virtual void HandleTranslationUnit(ASTContext &Context) override {
      Visitor.TraverseDecl(Context.getTranslationUnitDecl());
    }

  private:
    StateModificationVisitor Visitor;
};

class StateModificationAction : public ASTFrontendAction {
public:
  // `RC` (rewrittenCode) will be updated after a compiler pass
  StateModificationAction(std::string &RC) : RewrittenCode(RC) {}

  void EndSourceFileAction() override {
    SourceManager &SM = TheRewriter.getSourceMgr();
    // emit the rewrittenbuffer to a string
    RewrittenCode = "";
    llvm::raw_string_ostream *strStream = new llvm::raw_string_ostream(RewrittenCode);
    TheRewriter.getEditBuffer(SM.getMainFileID()).write(*strStream);
    llvm::errs() << "RewrittenCode: " << RewrittenCode << "\n";
  }

  virtual std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI, StringRef file) override {
    TheRewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
    return std::make_unique<StateModificationConsumer>(TheRewriter);
  }

private:
  Rewriter TheRewriter;
  std::string& RewrittenCode;
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
  std::string outputFile = "code.out";
  outputFile = argv[1];
  outputFile += ".out";

  std::stringstream buf;
  buf << file.rdbuf();
  std::string code = buf.str();
  std::string rewrittenCode;

  // run on input
  if (argc > 1) {
    clang::tooling::runToolOnCode(std::make_unique<StateModificationAction>(rewrittenCode), code);
  }

  std::ofstream fout;
  fout.open(outputFile);
  fout << rewrittenCode;
  fout.close();
  llvm::errs() << "MOdification ends: " << outputFile << " is the output code." << "\n";
  return 0;
}
