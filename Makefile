CLANGFULL := /usr/local/clang-ubuntu-20.04

BASE_PROGRAM_DIR := $$HOME/smartnic-rewriter
UNITTEST_DIR := $(BASE_PROGRAM_DIR)/progs_for_tests
UNITTEST_DIR_AT_LINUX_FOLDER := /usr/local/linux-5.4

# Other paths can be derived from the CLANGFULL path above.
# To run the sample as is, leave the rest of this Makefile
# unchanged.
LLVM_BUILD_PATH := $(CLANGFULL)
LLVM_BIN_PATH 	:= $(LLVM_BUILD_PATH)/bin

CXX := g++
CXXFLAGS := -fno-rtti -O0 -g

LLVM_CXXFLAGS := `$(LLVM_BIN_PATH)/llvm-config --cxxflags`
LLVM_LDFLAGS := `$(LLVM_BIN_PATH)/llvm-config --ldflags --libs --system-libs`
CLANG_INCLUDES :=

CLANG_LIBS := \
	-Wl,--start-group \
	-lclangAST \
	-lclangASTMatchers \
	-lclangAnalysis \
	-lclangBasic \
	-lclangDriver \
	-lclangEdit \
	-lclangFrontend \
	-lclangFrontendTool \
	-lclangLex \
	-lclangParse \
	-lclangSema \
	-lclangEdit \
	-lclangRewrite \
	-lclangRewriteFrontend \
	-lclangStaticAnalyzerFrontend \
	-lclangStaticAnalyzerCheckers \
	-lclangStaticAnalyzerCore \
	-lclangCrossTU \
	-lclangIndex \
	-lclangSerialization \
	-lclangToolingCore \
	-lclangTooling \
	-lclangFormat \
	-Wl,--end-group

COMPILER_LIBS := compilerPass.cpp changeStateVariable.cpp

TARGETS := rewriter

rewriter: rewriter.cpp $(COMPILER_LIBS)
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) $(CLANG_INCLUDES) $^ \
	$(CLANG_LIBS) $(LLVM_LDFLAGS) -o $@

state_trans_basic: state_trans_basic.cpp $(COMPILER_LIBS)
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) $(CLANG_INCLUDES) $^ \
	$(CLANG_LIBS) $(LLVM_LDFLAGS) -o $@
