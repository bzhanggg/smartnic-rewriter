CLANGFULL := $$HOME/smartnic-rewriter/clang+llvm-12.0.0-x86_64-linux-gnu-ubuntu-20.04/

BASE_PROGRAM_DIR := $$HOME/smartnic-rewriter
UNITTEST_DIR := $(BASE_PROGRAM_DIR)/progs_for_tests
UNITTEST_DIR_AT_LINUX_FOLDER := $(BASE_PROGRAM_DIR)/linux-5.4/samples/bpf

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

state_trans_basic: state_trans_basic.cpp
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) $(CLANG_INCLUDES) $^ \
	$(CLANG_LIBS) $(LLVM_LDFLAGS) -o $@
