#include <fstream>
#include "gtest/gtest.h"
#include "parser.hh"
#include "ParserDriver.hh"
using namespace std;
/**
 * Testing the parse tree is simple: just print the leaves of the tree (postorder), and we SHOULD end up with the exact
 * same input we started with. This way we can easily match the format of the reference lexer,
 * we just strip away the line numbers. For trivial terminals, we map the reference compiler's terminal name to the character.
 * For nontrivial terminals, we REMOVE the terminal type and include only the lexeme (whether it be an integer or word).
 * Since the parser can parse cLAsS as a class token, we will take care of converting to lowercase to match the reference
 * compiler.
 */

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


/**
 * This method is just putting each token on its own line while keeping the original text (mostly original text, case insensitive
 * words are converted to all lowercase by the reference lexer)
 * Example:
 *      class Main{};
 * would be turned into
 *      class
 *      Main
 *      (
 *      )
 *      ;
 * @param fileName
 * @return
 */
stringstream makeStringStreamFromReferenceAndFormatForParseTree(string fileName) {
    //generate the reference output, unformatted
    string command = CD + tests_EXE_TO_ROOT + RESOURCES_DIR_FROM_ROOT + " && ./cool --lex " + COOL_PROGRAMS_DIR_FROM_RESOURCES + fileName;
    system(command.c_str());

    //read reference output into an ifstream
    ifstream referenceLexerOutput(tests_EXE_TO_COOL_PROGRAMS + fileName + "-lex");

    stringstream s;
    string currLine;
    while(getline(referenceLexerOutput, currLine)) {
        //first line always discarded because it's a line number
        getline(referenceLexerOutput, currLine);
        if(currLine == "identifier" || currLine == "type" || currLine == "string" || currLine == "integer") {
            //discard the line, get the next line which is the lexeme associated with this terminal
            getline(referenceLexerOutput, currLine);
            s << currLine << endl;
        }
        else {  //character and keyword tokens
            if(tokenReqTranslation.find(currLine) != tokenReqTranslation.end()) {
                s << tokenReqTranslation[currLine] << endl;
            }
            else {
                s << currLine << endl;
            }
        }
    }

    return s;
}

/**
 * Test fixture for parseTree tests.
 */
class parseTreeTests : public testing::TestWithParam<string> {
protected:
    ParserDriver pdrv;
    stringstream parseTreeOutput;
    stringstream reference;

    void SetUp() override {
        //generate reference stringstream
        reference = makeStringStreamFromReferenceAndFormatForParseTree(GetParam());
    }

    void TearDown() override {

    }
};
TEST_P(parseTreeTests, matchesReferenceCompiler) {
    pdrv.parse(tests_EXE_TO_COOL_PROGRAMS + GetParam());
    pdrv.postorderTraversal(parseTreeOutput);

    ASSERT_EQ(reference.str(), parseTreeOutput.str());
}
INSTANTIATE_TEST_SUITE_P(Fragments, parseTreeTests, testing::Values(
                            "bareMinInput.cl",
                            "bareMinInputWithInh.cl",
                            "bareMinNoInhSingleFeature.cl",
                            "bareMinInhSingleFeature.cl",
                            "multipleClassesMultipleFields.cl",
                            "singleFieldInit.cl",
                            "twoFieldsOneInit.cl",
                            "singleMethodNoFormals.cl",
                            "singleMethodOneFormals.cl",
                            "singleMethodTwoFormals.cl",
                            "singleMethodFiveFormals.cl",
                            "assignExpr.cl",
                            "allDispatchesNoArguments.cl",
                            "allDispatchesOneArguments.cl",
                            "allDispatchesTwoArguments.cl",
                            "allDispatchesFiveArguments.cl",
                            "ifWhileExpressions.cl",
                            "blockExprNoExpressions.cl",
                            "blockExprOneExpressions.cl",
                            "blockExprTwoExpressions.cl",
                            "blockExprFiveExpressions.cl",
                            "letExprOneBindingNoInit.cl",
                            "letExprOneBindingYesInit.cl",
                            "letExprMultipleBindingMixedInit.cl",
                            "caseExprOneCase.cl",
                            "caseExprManyCase.cl",
                            "newExpr.cl",
                            "isvoidExpr.cl",
                            "integerPlusMinusTimesDivideExpressions.cl",
                            "relationalExpr.cl",
                            "notNegateExpr.cl",
                            "parenthesizedExpr.cl",
                            "stringExpr.cl",
                            "identifierExpr.cl",
                            "booleanExpr.cl"
                            ));
INSTANTIATE_TEST_SUITE_P(ConcreteParseFull, parseTreeTests, testing::Values(
                            "arith.cl",
                            "atoi.cl",
                            "cells.cl",
                            "hello-world.cl",
                            "hs.cl",
                            "list.cl",
                            "new-complex.cl",
                            "primes.cl",
                            "print-cool.cl",
                            "sort-list.cl"
));

