#include "gtest/gtest.h"
#include "utility.h"
#include "ParserDriver.hh"


using namespace std;

class semanticAnalyzerFixture : public testing::TestWithParam<string> {
protected:
    ParserDriver drv;
    const string TYPE = "--type";

    stringstream expected, actual;

    void SetUp() override {
        generateReference(expected, GetParam(), TYPE);
    }
};
TEST_P(semanticAnalyzerFixture, positive) {
    drv.parse(buildToResourcesPath + "CoolPrograms/" + GetParam());
    drv.buildInternalsAst();
    drv.buildEnvs();
    drv.populateClassImplementationMaps();

    drv.internalsAst->decorateInternals(drv.env);
    drv.ast->decorate(drv.env);

    drv.printClassMap(actual);
    drv.printImplementationMap(actual);
    drv.printParentMap(actual);

    drv.ast->print(actual);


    writeMyOutputToCoolPrograms(actual.str(), GetParam() + "-type-ivan");

    ASSERT_EQ(actual.str(), expected.str());
}
INSTANTIATE_TEST_SUITE_P(positiveSemanticAnalyzer, semanticAnalyzerFixture, testing::Values(
    "bareMinimum.cl"
));
//INSTANTIATE_TEST_SUITE_P(semanticAnalyzerFull, semanticAnalyzerFixture, testing::Values(
//    "CoolProgramsFull/arith.cl",
//    "CoolProgramsFull/atoi.cl",
//    "CoolProgramsFull/cells.cl",
//    "CoolProgramsFull/hello-world.cl",
//    "CoolProgramsFull/hs.cl",
//    "CoolProgramsFull/list.cl",
//    "CoolProgramsFull/new-complex.cl",
//    "CoolProgramsFull/primes.cl",
//    "CoolProgramsFull/print-cool.cl",
//    "CoolProgramsFull/sort-list.cl"
//));
