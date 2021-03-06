#include "gtest/gtest.h"
#include "utility.h"
#include "ParserDriver.hh"


using namespace std;

class astFixture : public testing::TestWithParam<string> {
protected:
    ParserDriver drv;
    const string PARSE = "--parse";

    stringstream expected, actual;

    void SetUp() override {
        generateReference(expected, GetParam(), PARSE);
    }
};
TEST_P(astFixture, positiveCases) {
    drv.parse(buildToResourcesPath + "CoolPrograms/" + GetParam());
    drv.ast->print(actual);

    ASSERT_EQ(actual.str(), expected.str());
}
INSTANTIATE_TEST_SUITE_P(positiveAst, astFixture, testing::Values(
    "bareMinimum.cl",
    "classInherits.cl",
    "singleAttrInit.cl",
    "singleAttrNoInit.cl",
    "methodNoFormals.cl",
    "methodOneFormal.cl",
    "methodTwoFormals.cl",
    "methodFiveFormals.cl",
    "severalAttrAndMethods.cl",
    "integerExpr.cl",
    "identifierExpr.cl",
    "stringExpr.cl",
    "selfDispatchNoArgs.cl",
    "selfDispatchOneArg.cl",
    "selfDispatchTwoArgs.cl",
    "selfDispatchFiveArgs.cl",
    "dynamicDispatchNoArgs.cl",
    "dynamicDispatchOneArg.cl",
    "dynamicDispatchTwoArgs.cl",
    "dynamicDispatchFiveArgs.cl",
    "staticDispatchNoArgs.cl",
    "staticDispatchOneArg.cl",
    "staticDispatchTwoArgs.cl",
    "staticDispatchFiveArgs.cl",
    "ifExpression.cl",
    "whileExpression.cl",
    "assignExpr.cl",
    "blockExprOneExpression.cl",
    "blockExprTwoExpressions.cl",
    "blockExprFiveExpressions.cl",
    "newExpr.cl",
    "isvoidExpr.cl",
    "integerPlusMinusTimesDivideExpressions.cl",
    "relationalExpr.cl",
    "notNegateExpr.cl",
    "parenthesizedExpr.cl",
    "letExprOneBindingNoInit.cl",
    "letExprOneBindingInit.cl",
    "letExprMultipleBindingsMixedInit.cl",
    "caseExprOneCase.cl",
    "caseExprManyCases.cl"
));
INSTANTIATE_TEST_SUITE_P(astFull, astFixture, testing::Values(
    "CoolProgramsFull/arith.cl",
    "CoolProgramsFull/atoi.cl",
    "CoolProgramsFull/cells.cl",
    "CoolProgramsFull/hello-world.cl",
    "CoolProgramsFull/hs.cl",
    "CoolProgramsFull/list.cl",
    "CoolProgramsFull/new-complex.cl",
    "CoolProgramsFull/primes.cl",
    "CoolProgramsFull/print-cool.cl",
    "CoolProgramsFull/sort-list.cl"
));
