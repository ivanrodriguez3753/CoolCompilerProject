#include <fstream>
#include "gtest/gtest.h"
#include "parser.hh"
#include "ParserDriver.hh"
//#include "lexerTests.hh"
using namespace std;

/**
 * Testing the parse tree is simple: just print the leaves of the tree (postorder), and we SHOULD end up with the exact
 * same input we started with, except we use token names. This way we can easily match the format of the reference lexer,
 * we just strip away the line numbers. For trivial terminals, we leave the terminal type name. For nontrivial terminals,
 * we REMOVE the terminal type and include only the lexeme (whether it be an integer or word).
 *
 *
 */




const string RESOURCES_DIR = "../../tests/resources/";
const string COOL_PROGRAMS_DIR = RESOURCES_DIR + "CoolPrograms/";


/**
 * This method is just putting each token on its own line while keeping the original text (trivial terminals turned into
 * the name of their token).
 * Example:
 *      class Main{};
 * would be turned into
 *      class
 *      Main
 *      lbrace
 *      rbrace
 *      semi
 * @param fileName
 * @return
 */
stringstream makeStringStreamFromReferenceAndFormatForParseTree(string fileName) {
    //generate the reference output
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
        }
        s << currLine << endl;
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