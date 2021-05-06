#include <iostream>

#include "gtest/gtest.h"
#include "utility.h"
#include "llvm/Support/raw_ostream.h"

#include "ParserDriver.hh"
using namespace std;

class codegenFixturePositive : public testing::TestWithParam<string> {
protected:

    stringstream expected, actual;
    string llContents;
    string llFileName;
    string outputFileName;

    void SetUp() override {
        generateReference(expected, GetParam(), "--full");
    }
    void TearDown() override {

    }
    void writeLLFile(ParserDriver& drv) {
        llFileName = buildToResourcesPath + "CoolPrograms/" + GetParam();
        llFileName.replace(llFileName.find(".cl"), 3, ".ll");
        ofstream llFile(llFileName);

        llvm::raw_string_ostream ostr(llContents);
        drv.llvmModule->print(ostr, nullptr);
        llFile << stringstream(llContents).rdbuf();
        llFile.close();
    }
    void LL_compileRunSave() {
        outputFileName = llFileName;
        outputFileName.replace(outputFileName.find(".ll"), 3, ".cl-out-ivan");
        string clangCmd = "clang " + llFileName;
        system(clangCmd.c_str());
        string aoutCmd = "./a.out > " + outputFileName;
        system(aoutCmd.c_str());
        string rm_aoutCmd = "rm -f a.out";
        system(rm_aoutCmd.c_str());
    }
    void loadOutputInStream() {
        ifstream ifs(outputFileName);
        actual << ifs.rdbuf();
    }
};
TEST_P(codegenFixturePositive, positive) {
    ParserDriver drv;

    drv.parse(buildToResourcesPath + "CoolPrograms/" + GetParam());
    drv.buildInternalsAst();
    drv.buildEnvs();
    drv.populateClassImplementationMaps();

    drv.decorateAST();

    drv.codegen();
    writeLLFile(drv);
    LL_compileRunSave();
    loadOutputInStream();

    ASSERT_EQ(actual.str(), expected.str());

}
INSTANTIATE_TEST_SUITE_P(positiveCodegen, codegenFixturePositive, testing::Values(
    "CoolProgramsAssembly/printIntLiteral.cl",
    "CoolProgramsAssembly/printIntAttrNoInit.cl",
    "CoolProgramsAssembly/printIntAttrInit.cl",
    "CoolProgramsAssembly/printIntParam.cl",
    "CoolProgramsAssembly/printFirstIntParam.cl",
    "CoolProgramsAssembly/printSecondIntParam.cl",
    "CoolProgramsAssembly/printStringLiteral.cl",
    "CoolProgramsAssembly/printStringAttrNoInit.cl",
    "CoolProgramsAssembly/printStringAttrInit.cl",
    "CoolProgramsAssembly/printStringParam.cl",
    "CoolProgramsAssembly/printFirstStringParam.cl",
    "CoolProgramsAssembly/printSecondStringParam.cl",
    "CoolProgramsAssembly/intBlockExpr.cl",
    "CoolProgramsAssembly/printLetInt.cl",
    "CoolProgramsAssembly/printLetIntNested.cl",
    "CoolProgramsAssembly/ifExpr.cl",
    "CoolProgramsAssembly/blockExpr.cl",
    "CoolProgramsAssembly/assignIntAttrExpr.cl",
    "CoolProgramsAssembly/assignIntFirstParamExpr.cl",
    "CoolProgramsAssembly/dynamicDispatch.cl",
    "CoolProgramsAssembly/methodOverride.cl",
    "CoolProgramsAssembly/intImmutability.cl",
    "CoolProgramsAssembly/chainedCopy.cl",
    "CoolProgramsAssembly/staticDispatch.cl",
    "CoolProgramsAssembly/isvoid.cl",
    "CoolProgramsAssembly/arith.cl",
    "CoolProgramsAssembly/relationalInt.cl",
    "CoolProgramsAssembly/relationalBool.cl",
    "CoolProgramsAssembly/relationalString.cl",
    "CoolProgramsAssembly/relationalVoid.cl",
    "CoolProgramsAssembly/relationalPointer.cl",
    "CoolProgramsAssembly/unaryNegate.cl",
    "CoolProgramsAssembly/unaryNot.cl",
    "CoolProgramsAssembly/while.cl",
    "CoolProgramsAssembly/whileReturnVoidObj.cl",
    "CoolProgramsAssembly/chainedCalls.cl",
    "CoolProgramsAssembly/usingBasePointers.cl",
    "CoolProgramsAssembly/case.cl",
    "CoolProgramsAssembly/typeName.cl",
    "CoolProgramsAssembly/substr.cl",
    "CoolProgramsAssembly/concat.cl",
    "CoolProgramsAssembly/length.cl"
));
INSTANTIATE_TEST_SUITE_P(codegenFull, codegenFixturePositive, testing::Values(
        "CoolProgramsFull/atoi.cl",
        "CoolProgramsFull/cells.cl",
        "CoolProgramsFull/hello-world.cl",
        "CoolProgramsFull/list.cl",
        "CoolProgramsFull/new-complex.cl",
        "CoolProgramsFull/primes.cl",
        "CoolProgramsFull/print-cool.cl",
        "CoolProgramsFull/hs.cl"
));
class codegenFixtureNegative : public testing::TestWithParam<string> {
protected:

    stringstream expected, actual;
    string llContents;
    string llFileName;
    string outputFileName;

    void SetUp() override {
        generateReference(expected, GetParam(), "--full");
    }
    void TearDown() override {

    }
    void writeLLFile(ParserDriver& drv) {
        llFileName = buildToResourcesPath + "CoolPrograms/" + GetParam();
        llFileName.replace(llFileName.find(".cl"), 3, ".ll");
        ofstream llFile(llFileName);

        llvm::raw_string_ostream ostr(llContents);
        drv.llvmModule->print(ostr, nullptr);
        llFile << stringstream(llContents).rdbuf();
        llFile.close();
    }
    void LL_compileRunSave() {
        outputFileName = llFileName;
        outputFileName.replace(outputFileName.find(".ll"), 3, ".cl-out-ivan");
        string clangCmd = "clang " + llFileName;
        system(clangCmd.c_str());
        string aoutCmd = "./a.out &> " + outputFileName;
        system(aoutCmd.c_str());
        string rm_aoutCmd = "rm -f a.out";
        system(rm_aoutCmd.c_str());
    }
    void loadOutputInStream() {
        ifstream ifs(outputFileName);
        actual << ifs.rdbuf();
    }
    pair<int, int> extractInts(stringstream& expected, stringstream& actual) {
        int e_int, a_int;
        expected >> e_int;
        actual >> a_int;

        //the reference compiler gives line number of the error at position 7, and this implementation
        //gives the line number of the error at the very end
        return {e_int, a_int};
    }
};
TEST_P(codegenFixtureNegative, negative) {
    ParserDriver drv;

    drv.parse(buildToResourcesPath + "CoolPrograms/" + GetParam());
    drv.buildInternalsAst();
    drv.buildEnvs();
    drv.populateClassImplementationMaps();

    drv.decorateAST();

    drv.codegen();
    writeLLFile(drv);
    LL_compileRunSave();
    loadOutputInStream();

    pair<int, int> errorLines = extractInts(expected, actual);

    ASSERT_EQ(errorLines.first, errorLines.second);

}
INSTANTIATE_TEST_SUITE_P(codegenNegative, codegenFixtureNegative, testing::Values(
        "CoolProgramsAssembly/RuntimeNegative/dynamicDispatchOnVoid.cl",
        "CoolProgramsAssembly/RuntimeNegative/staticDispatchOnVoid.cl",
        "CoolProgramsAssembly/RuntimeNegative/caseNoMatchingBranch.cl",
        "CoolProgramsAssembly/RuntimeNegative/caseVoidSwitchee.cl"


));
