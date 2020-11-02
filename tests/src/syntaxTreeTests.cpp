#include <fstream>
#include "gtest/gtest.h"
#include "parser.hh"
#include "ParserDriver.hh"
using namespace std;

const string RESOURCES_DIR = "../../tests/resources/";
const string COOL_PROGRAMS_DIR = RESOURCES_DIR + "CoolPrograms/";

stringstream makeSyntaxTreeStringStreamFromReference(string fileName) {
    //generate the reference output
    string command = "cd " + RESOURCES_DIR + " && ./cool --parse CoolPrograms/" + fileName;
    system(command.c_str());

    //read reference output into an ifstream
    string clLexFilePath = RESOURCES_DIR + "CoolPrograms/" + fileName + "-ast";
    ifstream referenceLexerOutput(clLexFilePath);

    //make it a stringstream so we can compare
    stringstream s;
    s << referenceLexerOutput.rdbuf();
    return s;
}

TEST(Fragments, syntaxTreeMinimal) {
    ParserDriver pdrv;
    const string localFile = "syntaxTreeMinimal.cl";
    pdrv.file = COOL_PROGRAMS_DIR + localFile;
    cout << pdrv.file << endl;

    stringstream parserOutput;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);
    _programNode* AST = (_programNode*) pdrv.buildSyntaxTree(rootIVAN);
    parserOutput << *AST;

    stringstream reference = makeSyntaxTreeStringStreamFromReference(localFile);
    ASSERT_EQ(reference.str(), parserOutput.str());
}