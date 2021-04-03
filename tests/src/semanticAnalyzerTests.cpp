#include "gtest/gtest.h"
#include "utility.h"
#include "ParserDriver.hh"


using namespace std;

class semanticAnalyzerFixturePositive : public testing::TestWithParam<string> {
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
TEST_P(semanticAnalyzerFixturePositive, positive) {
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


    writeMyOutputToCoolProgramsDir(actual.str(), GetParam() + "-type-ivan");

    ASSERT_EQ(actual.str(), expected.str());
}
INSTANTIATE_TEST_SUITE_P(positiveSemanticAnalyzer, semanticAnalyzerFixturePositive, testing::Values(
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
    "selfDispatchFiveArgs.cl",
    "staticDispatchNoArgs.cl",
    "staticDispatchOneArg.cl",
    "staticDispatchTwoArgs.cl",
    "staticDispatchFiveArgs.cl",
    "dynamicDispatchNoArgs.cl",
    "dynamicDispatchOneArg.cl",
    "dynamicDispatchTwoArgs.cl",
    "dynamicDispatchFiveArgs.cl"
));
INSTANTIATE_TEST_SUITE_P(semanticAnalyzerFull, semanticAnalyzerFixturePositive, testing::Values(
    "CoolProgramsFull/arith.cl",
    "CoolProgramsFull/atoi.cl",
    "CoolProgramsFull/cells.cl",
    "CoolProgramsFull/hello-world.cl",
    "CoolProgramsFull/list.cl",
    "CoolProgramsFull/new-complex.cl",
    "CoolProgramsFull/primes.cl",
    "CoolProgramsFull/print-cool.cl",
    "CoolProgramsFull/sort-list.cl",
    "CoolProgramsFull/hs.cl"
));
class semanticAnalyzerFixtureNegativeWithRef : public testing::TestWithParam<string> {
protected:
    //TODO: find out why only an instance ParserDriver is working instead of one with the fixture

    //using full because we want to save the stuff that shows up on the command line
    //which in this case is "ERROR: <lineNo>..."
    const string FULL = "--full";

    stringstream expected, actual;
    int expectedInt, actualInt;

    int extractInt(stringstream& ss) {
        string s = ss.str();
        return atoi(&s.at(7));
    }

    void SetUp() override {
        generateReference(expected, GetParam(), FULL);
        expectedInt = extractInt(expected);
    }
    void TearDown() override {

    }
};
TEST_P(semanticAnalyzerFixtureNegativeWithRef, negative) {
    ParserDriver drv;

    drv.parse(buildToResourcesPath + "CoolPrograms/" + GetParam());
    drv.buildInternalsAst();
    drv.buildEnvs();
    drv.populateClassImplementationMaps();

    drv.internalsAst->decorateInternals(drv.env);
    drv.ast->decorate(drv);

    writeMyOutputToCoolProgramsDir(actual.str(), GetParam() + "-type-ivan");

    if(drv.errorLog.size()) {
        actualInt = drv.errorLog[0].first;
        ASSERT_EQ(actualInt, expectedInt);
    }
    else {
        FAIL() << "Did not find any errors\n";
    }
}
INSTANTIATE_TEST_SUITE_P(negative, semanticAnalyzerFixtureNegativeWithRef, testing::Values(
    "CoolProgramsNegative/noMainClass.cl",
    "CoolProgramsNegative/noMainMethod.cl",
    "CoolProgramsNegative/noZeroArgMainMethod.cl"

));