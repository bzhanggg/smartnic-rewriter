#include "changeStateVariable.h"


ChangeStateVisitor::ChangeStateVisitor(Rewriter &R) : TheRewriter(R) {}

bool ChangeStateVisitor::VisitBinaryOperator(BinaryOperator *op) {
  if (op->getOpcode() == BO_Assign || op->isCompoundAssignmentOp()) {
    DeclRefExpr *lhs = dyn_cast<DeclRefExpr>(op->getLHS());
    // for now, we hardcode find out if the variable being modified has identifier "state"
    if (lhs && lhs->getNameInfo().getAsString() == "state") {
      rewriteStateChange(op);
    }
  }
  return true;
}

bool ChangeStateVisitor::VisitUnaryOperator(UnaryOperator *op) {
  if (op->isIncrementDecrementOp()) {
    DeclRefExpr *operand = dyn_cast<DeclRefExpr>(op->getSubExpr());
    // for now, we hardcode find out if the variable being modified has identifier "state"
    if (operand && operand->getNameInfo().getAsString() == "state") {
      rewriteStateChange(op);
    }
  }
  return true;
}

// hardcode the rewrite to loopify a single state modification
void ChangeStateVisitor::rewriteStateChange(clang::Stmt *StateChangingStmt) {
  clang::SourceRange range = StateChangingStmt->getSourceRange();
  clang::SourceManager &sourceManager = TheRewriter.getSourceMgr();
  LangOptions langOptions = TheRewriter.getLangOpts();

  CharSourceRange charRange = CharSourceRange::getTokenRange(range);
  std::string originalText = Lexer::getSourceText(charRange, sourceManager, langOptions).str();

  std::string transformedText = "for (int i = 0; i < NUM_PKTS - 1; i++) {\n\t\t" + originalText + ";\n\t}\n\t" + originalText;
  TheRewriter.ReplaceText(range, transformedText);
}
