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

stringstream makeStringStreamFromReference(string fileName) {
    //generate the reference output
    string command = CD + tests_EXE_TO_ROOT + RESOURCES_DIR_FROM_ROOT + " && ./cool --lex " + COOL_PROGRAMS_DIR_FROM_RESOURCES + fileName;
    system(command.c_str());

    //read reference output into an ifstream
    string clLexFilePath = tests_EXE_TO_COOL_PROGRAMS + fileName + "-lex";
    ifstream referenceLexerOutput(clLexFilePath);

    //make it a stringstream so we can compare
    stringstream s;
    s << referenceLexerOutput.rdbuf();
    return s;
}
void lexInput(ParserDriver& pdrv, stringstream& lexerOutput) {
    pdrv.scan_begin();
    string tokenType = "";
    while(tokenType != "end of file") { //the token name that bison generates for <EOF> token
        yy::parser::symbol_type current{yylex(pdrv)};
        tokenType = current.name();
        if(tokenType != "end of file") {
            lexerOutput << current.location.begin.line << '\n';
            lexerOutput << current.name() << endl;

            if (tokenType == "identifier" || tokenType == "type" || tokenType == "string") {
                lexerOutput << current.value.as<string>() << endl;
            } else if (tokenType == "integer") {
                lexerOutput << current.value.as<int>() << endl;
            }
        }
    }
}

/**
 * Test fixture for lexerFull tests.
 */
class lexerTests : public testing::TestWithParam<string> {
protected:
    ParserDriver pdrv;
    stringstream lexerOutput;
    stringstream reference;

    void SetUp() override {
        pdrv.file = tests_EXE_TO_COOL_PROGRAMS + GetParam();

        reference = makeStringStreamFromReference(GetParam());
    }

    void TearDown() override {

    }
};
TEST_P(lexerTests, matchesReferenceCompiler) {
    lexInput(pdrv, lexerOutput);
    ASSERT_EQ(reference.str(), lexerOutput.str());
}
INSTANTIATE_TEST_SUITE_P(LexerFull, lexerTests, testing::Values(
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
