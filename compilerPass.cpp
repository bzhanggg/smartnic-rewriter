#include "compilerPass.h"

/**
 * reads the AST produced by the Clang parser
 */
template<typename T> class GenericASTConsumer : public ASTConsumer {
public:
  GenericASTConsumer(Rewriter &R) : Visitor(R) {}

  // override the top-level declaration to be able to traverse into it
  bool HandleTopLevelDecl(DeclGroupRef DR) override {
    for (DeclGroupRef::iterator b = DR.begin(), e = DR.end(); b != e; ++b) {
      Visitor.TraverseDecl(*b);
    }
    return true;
  }

  // TODO: figure out how this plays into the state_trans_basic.cpp file
  // see link: https://stackoverflow.com/questions/37802947/visitdecl-vs-traversedecl-clang-recursiveastvisitor
  virtual void HandleTranslationUnit(ASTContext &Context) override {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }

private:
  T Visitor;
};

/**
 * Every source file provided to the Tool is given a new FrontendAction
 */
template<typename T> class GenericFrontendAction : public ASTFrontendAction {
public:
  // RC is to be updated after a compiler pass
  GenericFrontendAction(std::string &RC) : RewrittenCode(RC) {}

  void EndSourceFileAction() override {
    SourceManager &SM = TheRewriter.getSourceMgr();
    // emite the rewritten buffer to a string
    RewrittenCode = "";
    llvm::raw_ostream *strStream = new llvm::raw_string_ostream(RewrittenCode);
    TheRewriter.getEditBuffer(SM.getMainFileID()).write(*strStream);
    llvm::errs() << "RewrittenCode: \n" << RewrittenCode << "\n";
  }

  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI, StringRef file) override {
    TheRewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
    return std::make_unique<GenericASTConsumer<T>>(TheRewriter);
  }

private:
  Rewriter TheRewriter;
  std::string& RewrittenCode;
};

/**
 * Creates a factory that returns a new GenericFrontendAction object every time
 * template T: the class name of the ASTVisitor used to modify the code
 * \param RC the rewritten code
 * \return a SimpleFrontendActionFactory that builds a new FrontendAction every time it is called
 */
template<typename T> std::unique_ptr<FrontendActionFactory> GenericFrontendActionFactory(std::string& RC) {
  class SimpleFrontendActionFactory : public FrontendActionFactory {
  public:
    SimpleFrontendActionFactory(std::string &RC) : RewrittenCode(RC) {}

    std::unique_ptr<FrontendAction> create() override {
      return std::make_unique<T>(RewrittenCode);
    }

  private:
    std::string& RewrittenCode;
  };

  return std::unique_ptr<FrontendActionFactory>(new SimpleFrontendActionFactory(RC));
}

/**
 * Run a compiler pass to modify code
 * template T: the class name of the ASTVisitor used to modify the code
 * \param sourceCode: the source code to be modified
 * \return the modified code after a compiler pass
 */
template<typename T> std::string compilerPass(const std::string& sourceCode) {
  std::string rewrittenCode;
  // run a FrontendAction over the sourceCode snippet
  clang::tooling::runToolOnCode(std::make_unique<GenericFrontendAction<T>>(rewrittenCode),
                                (const Twine&)sourceCode);
  return rewrittenCode;
}

/**
 * Run a compiler pass to modify code
 * template T: the class name of the ASTVisitor used to modify the code
 * \param argc: the number of arguments passed
 * \param argv: the array of character pointers listing all args
 * \return The modified code after a compiler pass
 */
template<typename T> std::string compilerPass(int argc, const char **argv) {
  CommonOptionsParser op(argc, argv, ToolingSampleCategory);
  ClangTool Tool(op.getCompilations(), op.getSourcePathList());

  std::string rewrittenCode;
  Tool.run(GenericFrontendActionFactory<GenericFrontendAction<T>>(rewrittenCode).get());
  return rewrittenCode;
}