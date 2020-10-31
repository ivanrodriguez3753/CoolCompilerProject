#include <fstream>
#include "gtest/gtest.h"
#include "parser.hh"
#include "ParserDriver.hh"
//#include "lexerTests.hh"
using namespace std;

/**
 * Testing the parse tree is simple: just print the leaves of the tree (postorder), and we SHOULD end up with the exact
 * same input we started with. This way we can easily match the format of the reference lexer,
 * we just strip away the line numbers. For trivial terminals, we map the reference compiler's terminal name to the character.
 * For nontrivial terminals, we REMOVE the terminal type and include only the lexeme (whether it be an integer or word).
 * Since the parser can parse cLAsS as a class token, we will take care of converting to lowercase to match the reference
 * compiler.
 */




const string RESOURCES_DIR = "../../tests/resources/";
const string COOL_PROGRAMS_DIR = RESOURCES_DIR + "CoolPrograms/";


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
    string command = "cd " + RESOURCES_DIR + " && ./cool --lex CoolPrograms/" + fileName;
    system(command.c_str());

    //read reference output into an ifstream
    string clLexFilePath = RESOURCES_DIR + "CoolPrograms/" + fileName + "-lex";
    ifstream referenceLexerOutput(clLexFilePath);

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


TEST(Fragments, bareMin) {
    const string localFile = "bareMinInput.cl";
    stringstream reference = makeStringStreamFromReferenceAndFormatForParseTree(localFile);

    ParserDriver pdrv;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);

    stringstream parserOutput;
    pdrv.postorderTraversal(parserOutput);

    ASSERT_EQ(reference.str(), parserOutput.str());
}

TEST(Fragments, bareMinWithInh) {
    const string localFile = "bareMinInputWithInh.cl";
    stringstream reference = makeStringStreamFromReferenceAndFormatForParseTree(localFile);

    ParserDriver pdrv;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);

    stringstream parserOutput;
    pdrv.postorderTraversal(parserOutput);

    ASSERT_EQ(reference.str(), parserOutput.str());
}

TEST(Fragments, bareMinNoInhSingleFeatureField) {
    const string localFile = "bareMinNoInhSingleFeature.cl";
    stringstream reference = makeStringStreamFromReferenceAndFormatForParseTree(localFile);

    ParserDriver pdrv;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);

    stringstream parserOutput;
    pdrv.postorderTraversal(parserOutput);

    ASSERT_EQ(reference.str(), parserOutput.str());
}

TEST(Fragments, bareMinInhSingleFeatureField) {
    const string localFile = "bareMinInhSingleFeature.cl";
    stringstream reference = makeStringStreamFromReferenceAndFormatForParseTree(localFile);

    ParserDriver pdrv;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);

    stringstream parserOutput;
    pdrv.postorderTraversal(parserOutput);

    ASSERT_EQ(reference.str(), parserOutput.str());
}

TEST(Fragments, multipleClassesMultipleFields) {
    const string localFile = "multipleClassesMultipleFields.cl";
    stringstream reference = makeStringStreamFromReferenceAndFormatForParseTree(localFile);

    ParserDriver pdrv;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);

    stringstream parserOutput;
    pdrv.postorderTraversal(parserOutput);

    ASSERT_EQ(reference.str(), parserOutput.str());
}

TEST(Fragments, singleFieldInit) {
    const string localFile = "singleFieldInit.cl";
    stringstream reference = makeStringStreamFromReferenceAndFormatForParseTree(localFile);

    ParserDriver pdrv;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);

    stringstream parserOutput;
    pdrv.postorderTraversal(parserOutput);

    ASSERT_EQ(reference.str(), parserOutput.str());
}

TEST(Fragments, twoFieldsOneInit) {
    const string localFile = "twoFieldsOneInit.cl";
    stringstream reference = makeStringStreamFromReferenceAndFormatForParseTree(localFile);

    ParserDriver pdrv;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);

    stringstream parserOutput;
    pdrv.postorderTraversal(parserOutput);

    ASSERT_EQ(reference.str(), parserOutput.str());
}

TEST(Fragments, singleMethodNoFormals) {
    const string localFile = "singleMethodNoFormals.cl";
    stringstream reference = makeStringStreamFromReferenceAndFormatForParseTree(localFile);

    ParserDriver pdrv;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);

    stringstream parserOutput;
    pdrv.postorderTraversal(parserOutput);
    pdrv.prettyPrintTree(cout);

    ASSERT_EQ(reference.str(), parserOutput.str());
}

TEST(Fragments, singleMethodOneFormals) {
    const string localFile = "singleMethodOneFormals.cl";
    stringstream reference = makeStringStreamFromReferenceAndFormatForParseTree(localFile);

    ParserDriver pdrv;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);

    stringstream parserOutput;
    pdrv.postorderTraversal(parserOutput);

    ASSERT_EQ(reference.str(), parserOutput.str());
}

TEST(Fragments, singleMethodTwoFormals) {
    const string localFile = "singleMethodTwoFormals.cl";
    stringstream reference = makeStringStreamFromReferenceAndFormatForParseTree(localFile);

    ParserDriver pdrv;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);

    stringstream parserOutput;
    pdrv.postorderTraversal(parserOutput);

    pdrv.prettyPrintTree(cout);

    ASSERT_EQ(reference.str(), parserOutput.str());
}

TEST(Fragments, singleMethodFiveFormals) {
    const string localFile = "singleMethodFiveFormals.cl";
    stringstream reference = makeStringStreamFromReferenceAndFormatForParseTree(localFile);

    ParserDriver pdrv;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);

    stringstream parserOutput;
    pdrv.postorderTraversal(parserOutput);

    pdrv.prettyPrintTree(cout);

    ASSERT_EQ(reference.str(), parserOutput.str());
}

TEST(Fragments, assignExpr) {
    const string localFile = "assignExpr.cl";
    stringstream reference = makeStringStreamFromReferenceAndFormatForParseTree(localFile);

    ParserDriver pdrv;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);

    stringstream parserOutput;
    pdrv.postorderTraversal(parserOutput);

    pdrv.prettyPrintTree(cout);

    ASSERT_EQ(reference.str(), parserOutput.str());
}

TEST(Fragments, allDispatchesNoArguments) {
    const string localFile = "allDispatchesNoArguments.cl";
    stringstream reference = makeStringStreamFromReferenceAndFormatForParseTree(localFile);

    ParserDriver pdrv;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);

    stringstream parserOutput;
    pdrv.postorderTraversal(parserOutput);


    ASSERT_EQ(reference.str(), parserOutput.str());
}

TEST(Fragments, allDispatchesOneArguments) {
    const string localFile = "allDispatchesOneArguments.cl";
    stringstream reference = makeStringStreamFromReferenceAndFormatForParseTree(localFile);

    ParserDriver pdrv;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);

    stringstream parserOutput;
    pdrv.postorderTraversal(parserOutput);

    ASSERT_EQ(reference.str(), parserOutput.str());
}

TEST(Fragments, allDispatchesTwoArguments) {
    const string localFile = "allDispatchesTwoArguments.cl";
    stringstream reference = makeStringStreamFromReferenceAndFormatForParseTree(localFile);

    ParserDriver pdrv;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);

    stringstream parserOutput;
    pdrv.postorderTraversal(parserOutput);

    ASSERT_EQ(reference.str(), parserOutput.str());
}

TEST(Fragments, allDispatchesFiveArguments) {
    const string localFile = "allDispatchesFiveArguments.cl";
    stringstream reference = makeStringStreamFromReferenceAndFormatForParseTree(localFile);

    ParserDriver pdrv;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);

    stringstream parserOutput;
    pdrv.postorderTraversal(parserOutput);

    ASSERT_EQ(reference.str(), parserOutput.str());
}

TEST(Fragments, ifWhileExpressions) {
    const string localFile = "ifWhileExpressions.cl";
    stringstream reference = makeStringStreamFromReferenceAndFormatForParseTree(localFile);

    ParserDriver pdrv;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);

    stringstream parserOutput;
    pdrv.postorderTraversal(parserOutput);

    ASSERT_EQ(reference.str(), parserOutput.str());
}

TEST(Fragments, blockExprNoExpressions) {
    const string localFile = "blockExprNoExpressions.cl";
    stringstream reference = makeStringStreamFromReferenceAndFormatForParseTree(localFile);

    ParserDriver pdrv;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);

    stringstream parserOutput;
    pdrv.postorderTraversal(parserOutput);
    cout << parserOutput.str();
    ASSERT_EQ(reference.str(), parserOutput.str());
}

TEST(Fragments, blockExprOneExpressions) {
    const string localFile = "blockExprOneExpressions.cl";
    stringstream reference = makeStringStreamFromReferenceAndFormatForParseTree(localFile);

    ParserDriver pdrv;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);

    stringstream parserOutput;
    pdrv.postorderTraversal(parserOutput);
    cout << parserOutput.str();
    ASSERT_EQ(reference.str(), parserOutput.str());
}

TEST(Fragments, blockExprTwoExpressions) {
    const string localFile = "blockExprTwoExpressions.cl";
    stringstream reference = makeStringStreamFromReferenceAndFormatForParseTree(localFile);

    ParserDriver pdrv;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);

    stringstream parserOutput;
    pdrv.postorderTraversal(parserOutput);
    cout << parserOutput.str();
    ASSERT_EQ(reference.str(), parserOutput.str());
}

TEST(Fragments, blockExprFiveExpressions) {
    const string localFile = "blockExprFiveExpressions.cl";
    stringstream reference = makeStringStreamFromReferenceAndFormatForParseTree(localFile);

    ParserDriver pdrv;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);

    stringstream parserOutput;
    pdrv.postorderTraversal(parserOutput);
    cout << parserOutput.str();
    ASSERT_EQ(reference.str(), parserOutput.str());
}

TEST(Fragments, letExprOneBindingNoInit) {
    const string localFile = "letExprOneBindingNoInit.cl";
    stringstream reference = makeStringStreamFromReferenceAndFormatForParseTree(localFile);

    ParserDriver pdrv;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);

    stringstream parserOutput;
    pdrv.postorderTraversal(parserOutput);
    cout << parserOutput.str();
    ASSERT_EQ(reference.str(), parserOutput.str());
}

TEST(Fragments, letExprOneBindingYesInit) {
    const string localFile = "letExprOneBindingYesInit.cl";
    stringstream reference = makeStringStreamFromReferenceAndFormatForParseTree(localFile);

    ParserDriver pdrv;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);

    stringstream parserOutput;
    pdrv.postorderTraversal(parserOutput);
    cout << parserOutput.str();
    ASSERT_EQ(reference.str(), parserOutput.str());
}

TEST(Fragments, letExprMultipleBindingMixedInit) {
    const string localFile = "letExprMultipleBindingMixedInit.cl";
    stringstream reference = makeStringStreamFromReferenceAndFormatForParseTree(localFile);

    ParserDriver pdrv;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);

    stringstream parserOutput;
    pdrv.postorderTraversal(parserOutput);
    cout << parserOutput.str();
    ASSERT_EQ(reference.str(), parserOutput.str());
}

TEST(Fragments, caseExprOneCase) {
    const string localFile = "caseExprOneCase.cl";
    stringstream reference = makeStringStreamFromReferenceAndFormatForParseTree(localFile);

    ParserDriver pdrv;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);

    stringstream parserOutput;
    pdrv.postorderTraversal(parserOutput);
    cout << parserOutput.str();
    ASSERT_EQ(reference.str(), parserOutput.str());
}

TEST(Fragments, caseExprManyCase) {
    const string localFile = "caseExprManyCase.cl";
    stringstream reference = makeStringStreamFromReferenceAndFormatForParseTree(localFile);

    ParserDriver pdrv;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);

    stringstream parserOutput;
    pdrv.postorderTraversal(parserOutput);
    cout << parserOutput.str();
    ASSERT_EQ(reference.str(), parserOutput.str());
}

TEST(ParserFull, arith) {
    const string localFile = "arith.cl";
    stringstream reference = makeStringStreamFromReferenceAndFormatForParseTree(localFile);

    ParserDriver pdrv;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);

    stringstream parserOutput;
    pdrv.postorderTraversal(parserOutput);
    ASSERT_EQ(reference.str(), parserOutput.str());
}

TEST(ParserFull, atoi) {
    const string localFile = "atoi.cl";
    stringstream reference = makeStringStreamFromReferenceAndFormatForParseTree(localFile);

    ParserDriver pdrv;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);

    stringstream parserOutput;
    pdrv.postorderTraversal(parserOutput);
    ASSERT_EQ(reference.str(), parserOutput.str());
}

TEST(ParserFull, cells) {
    const string localFile = "cells.cl";
    stringstream reference = makeStringStreamFromReferenceAndFormatForParseTree(localFile);

    ParserDriver pdrv;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);

    stringstream parserOutput;
    pdrv.postorderTraversal(parserOutput);
    ASSERT_EQ(reference.str(), parserOutput.str());
}

TEST(ParserFull, helloworld) {
    const string localFile = "hello-world.cl";
    stringstream reference = makeStringStreamFromReferenceAndFormatForParseTree(localFile);

    ParserDriver pdrv;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);

    stringstream parserOutput;
    pdrv.postorderTraversal(parserOutput);
    ASSERT_EQ(reference.str(), parserOutput.str());
}

TEST(ParserFull, hs) {
    const string localFile = "hs.cl";
    stringstream reference = makeStringStreamFromReferenceAndFormatForParseTree(localFile);

    ParserDriver pdrv;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);

    stringstream parserOutput;
    pdrv.postorderTraversal(parserOutput);
    ASSERT_EQ(reference.str(), parserOutput.str());
}

TEST(ParserFull, list) {
    const string localFile = "list.cl";
    stringstream reference = makeStringStreamFromReferenceAndFormatForParseTree(localFile);

    ParserDriver pdrv;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);

    stringstream parserOutput;
    pdrv.postorderTraversal(parserOutput);
    ASSERT_EQ(reference.str(), parserOutput.str());
}

TEST(ParserFull, newcomplex) {
    const string localFile = "new-complex.cl";
    stringstream reference = makeStringStreamFromReferenceAndFormatForParseTree(localFile);

    ParserDriver pdrv;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);

    stringstream parserOutput;
    pdrv.postorderTraversal(parserOutput);
    ASSERT_EQ(reference.str(), parserOutput.str());
}

TEST(ParserFull, primes) {
    const string localFile = "primes.cl";
    stringstream reference = makeStringStreamFromReferenceAndFormatForParseTree(localFile);

    ParserDriver pdrv;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);

    stringstream parserOutput;
    pdrv.postorderTraversal(parserOutput);
    ASSERT_EQ(reference.str(), parserOutput.str());
}

TEST(ParserFull, printcool) {
    const string localFile = "print-cool.cl";
    stringstream reference = makeStringStreamFromReferenceAndFormatForParseTree(localFile);

    ParserDriver pdrv;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);

    stringstream parserOutput;
    pdrv.postorderTraversal(parserOutput);
    ASSERT_EQ(reference.str(), parserOutput.str());
}

TEST(ParserFull, sortlist) {
    const string localFile = "sort-list.cl";
    stringstream reference = makeStringStreamFromReferenceAndFormatForParseTree(localFile);

    ParserDriver pdrv;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);

    stringstream parserOutput;
    pdrv.postorderTraversal(parserOutput);
    ASSERT_EQ(reference.str(), parserOutput.str());
}