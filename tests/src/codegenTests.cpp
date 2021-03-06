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



}
INSTANTIATE_TEST_SUITE_P(positiveCodegen, codegenFixture, testing::Values(
//        "hello-world.cl"
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
