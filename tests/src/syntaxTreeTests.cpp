#include <fstream>
#include "gtest/gtest.h"
#include "parser.hh"
#include "ParserDriver.hh"
using namespace std;

/**
 * Currently, the googletest tests executable is in /CoolCompilerProject/cmake-build-debug/tests. Going to root
 * of project is two directories back
 */
const string tests_EXE_TO_ROOT = "../../";
const string RESOURCES_DIR_FROM_ROOT = "tests/resources/";
const string COOL_PROGRAMS_DIR_FROM_RESOURCES = "CoolPrograms/";
const string tests_EXE_TO_COOL_PROGRAMS = tests_EXE_TO_ROOT + RESOURCES_DIR_FROM_ROOT + COOL_PROGRAMS_DIR_FROM_RESOURCES;
const string CD = "cd ";

//const string RESOURCES_DIR = "../../tests/resources/";
//const string COOL_PROGRAMS_DIR = RESOURCES_DIR + "CoolPrograms/";

stringstream makeSyntaxTreeStringStreamFromReference(string fileName) {
    //generate the reference output
    string commandGenerateReference = CD + tests_EXE_TO_ROOT + RESOURCES_DIR_FROM_ROOT + " && ./cool --parse " + COOL_PROGRAMS_DIR_FROM_RESOURCES + fileName;
    system(commandGenerateReference.c_str());

    //read reference output into an ifstream
    ifstream referenceLexerOutput(tests_EXE_TO_COOL_PROGRAMS + fileName + "-ast");

    //make it a stringstream so we can compare
    stringstream s;
    s << referenceLexerOutput.rdbuf();
    return s;
}

/**
 * Test fixture for classMap tests.
 */
class SyntaxTreeTests : public testing::TestWithParam<string> {
protected:
    ParserDriver pdrv;
    stringstream parserOutput;
    stringstream reference;

    void SetUp() override {
        //parse the input into global parse tree rootIVAN
        pdrv.parse(tests_EXE_TO_COOL_PROGRAMS + GetParam());

        reference = makeSyntaxTreeStringStreamFromReference(GetParam());
    }

    void TearDown() override {
        globalEnv->reset();
    }
};
TEST_P(SyntaxTreeTests, matchesReferenceCompiler) {
    //build syntax tree out of rootIVAN concreteParseTree
    _program* AST = (_program*) pdrv.buildSyntaxTree(rootIVAN);
    parserOutput << *AST;

    ASSERT_EQ(reference.str(), parserOutput.str());
}
INSTANTIATE_TEST_SUITE_P(Fragments, SyntaxTreeTests, testing::Values(
                            "syntaxTreeMinimal.cl",
                            "classInherits.cl",
                            "singleFieldInit.cl",
                            "singleMethodNoFormals.cl",
                            "singleMethodOneFormals.cl",
                            "singleMethodTwoFormals.cl",
                            "singleMethodFiveFormals.cl",
                            "allDispatchesNoArguments.cl",
                            "allDispatchesOneArguments.cl",
                            "allDispatchesTwoArguments.cl",
                            "allDispatchesFiveArguments.cl",
                            "ifWhileExpressions.cl",
                            "assignExpr.cl",
                            "blockExprOneExpressions.cl",
                            "blockExprTwoExpressions.cl",
                            "blockExprFiveExpressions.cl",
                            "newExpr.cl",
                            "isvoidExpr.cl",
                            "integerPlusMinusTimesDivideExpressions.cl",
                            "relationalExpr.cl",
                            "notNegateExpr.cl",
                            "parenthesizedExpr.cl",
                            "stringExpr.cl",
                            "identifierExpr.cl",
                            "booleanExpr.cl",
                            "letExprOneBindingNoInit.cl",
                            "letExprOneBindingYesInit.cl",
                            "letExprMultipleBindingMixedInit.cl",
                            "caseExprOneCase.cl",
                            "caseExprManyCase.cl"));
INSTANTIATE_TEST_SUITE_P(AbstractParseFull, SyntaxTreeTests, testing::Values(
                            "arith.cl",
                            "atoi.cl",
                            "cells.cl",
                            "hello-world.cl",
                            "hs.cl",
                            "list.cl",
                            "new-complex.cl",
                            "primes.cl",
                            "print-cool.cl",
                            "sort-list.cl"));
