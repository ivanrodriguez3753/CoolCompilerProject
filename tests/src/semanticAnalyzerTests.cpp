#include "gtest/gtest.h"
#include "utility.h"
#include "ParserDriver.hh"


using namespace std;

class semanticAnalyzerFixture : public testing::TestWithParam<string> {
protected:
    //TODO: find out why only an instance ParserDriver is working instead of one with the fixture
    //ParserDriver drv

    const string TYPE = "--type";

    stringstream expected, actual;

    void SetUp() override {
        generateReference(expected, GetParam(), TYPE);
    }
    void TearDown() override {

    }
};
TEST_P(semanticAnalyzerFixture, positive) {
    //TODO: find out why only an instance ParserDriver is working instead of one with the fixture
    ParserDriver drv;

    drv.parse(buildToResourcesPath + "CoolPrograms/" + GetParam());
    drv.buildInternalsAst();
    drv.buildEnvs();
    drv.populateClassImplementationMaps();

    drv.internalsAst->decorateInternals(drv.env);
    drv.ast->decorate(drv);

    drv.printClassMap(actual);
    drv.printImplementationMap(actual);
    drv.printParentMap(actual);

    drv.ast->print(actual);


    writeMyOutputToCoolPrograms(actual.str(), GetParam() + "-type-ivan");

    ASSERT_EQ(actual.str(), expected.str());
}
INSTANTIATE_TEST_SUITE_P(positiveSemanticAnalyzer, semanticAnalyzerFixture, testing::Values(
    "bareMinimum.cl",
    "classMapNoInitializations.cl",
    "classMapNoInitializationsWith2Inheritance.cl",
    "letExprOneBindingNoInit.cl",
    "letExprOneBindingInit.cl",
    "letExprMultipleBindingsMixedInit.cl",
    "letExprTricky.cl",
    "blockExprFiveExpressions.cl",
    "assignExpr.cl",
    "assignTricky.cl",
    "newExpr.cl",
    "caseExprOneCase.cl",
    "caseExprManyCases.cl",
    "caseExprTricky.cl",
    "ifExpression.cl",
    "whileExpression.cl",
    "isvoidExpr.cl",
    "identifierExpr.cl",
    "selfDispatchNoArgs.cl",
    "selfDispatchOneArg.cl",
    "selfDispatchTwoArgs.cl",
    "selfDispatchFiveArgs.cl"
//    "staticDispatchExpr.cl"
//    "staticDispatchExpr2.cl",
//    "dynamicDispatchExpr.cl",
//    "SELF_TYPE.cl",

//    "assignMismatchButConforms.cl"
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
