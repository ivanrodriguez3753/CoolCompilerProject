#include <iostream>

#include "gtest/gtest.h"
#include "utility.h"
#include "llvm/Support/raw_ostream.h"

#include "ParserDriver.hh"
using namespace std;

class codegenFixture : public testing::TestWithParam<string> {
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
TEST_P(codegenFixture, positive) {
    ParserDriver drv;

    drv.parse(buildToResourcesPath + "CoolPrograms/" + GetParam());
    drv.buildInternalsAst();
    drv.buildEnvs();
    drv.populateClassImplementationMaps();

    drv.internalsAst->decorateInternals(drv.env); //TODO: make a driver call
    drv.ast->decorate(drv); //TODO: same. wrap in a driver call

    drv.codegen();
    writeLLFile(drv);
    LL_compileRunSave();
    loadOutputInStream();

    ASSERT_EQ(actual.str(), expected.str());

}
INSTANTIATE_TEST_SUITE_P(positiveCodegen, codegenFixture, testing::Values(
    "CoolProgramsAssembly/printIntLiteral.cl",
    "CoolProgramsAssembly/printIntAttrNoInit.cl",
    "CoolProgramsAssembly/printIntAttrInit.cl",
    "CoolProgramsAssembly/printIntParam.cl",
    "CoolProgramsAssembly/printFirstIntParam.cl",
    "CoolProgramsAssembly/printSecondIntParam.cl",
    "CoolProgramsAssembly/intBlockExpr.cl",
    "CoolProgramsAssembly/printLetInt.cl",
    "CoolProgramsAssembly/printLetIntNested.cl"

//    "CoolProgramsAssembly/ifExpr.cl",
//    "CoolProgramsAssembly/blockExpr.cl"
));
INSTANTIATE_TEST_SUITE_P(codegenFull, codegenFixture, testing::Values(
//        "CoolProgramsFull/arith.cl",
//        "CoolProgramsFull/atoi.cl",
//        "CoolProgramsFull/cells.cl",
        "CoolProgramsFull/hello-world.cl"
//        "CoolProgramsFull/list.cl",
//        "CoolProgramsFull/new-complex.cl",
//        "CoolProgramsFull/primes.cl",
//        "CoolProgramsFull/print-cool.cl",
//        "CoolProgramsFull/sort-list.cl",
//        "CoolProgramsFull/hs.cl"
));
