#include <iostream>

#include "gtest/gtest.h"
#include "utility.h"
#include "codegen.h"

#include "ParserDriver.hh"
using namespace std;

class codegenFixture : public testing::TestWithParam<string> {
protected:

    stringstream expected, actual;

    void SetUp() override {

    }
    void TearDown() override {

    }
};
TEST_P(codegenFixture, positive) {
    ParserDriver drv;

    drv.parse(buildToResourcesPath + "CoolPrograms/" + GetParam());
    drv.buildInternalsAst();
    drv.buildEnvs();
    drv.populateClassImplementationMaps();

    drv.internalsAst->decorateInternals(drv.env);
    drv.ast->decorate(drv);

    codegenDriver cdrv(drv);


    string result;
    llvm::raw_string_ostream ss(result);
    cdrv.llvmModule->print(ss, nullptr);
    cout << "$$$" + ss.str() + "$$$\n";
}
INSTANTIATE_TEST_SUITE_P(positiveCodegen, codegenFixture, testing::Values(
        "bareMinimum.cl"
));
