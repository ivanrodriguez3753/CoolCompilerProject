#include <fstream>
#include "gtest/gtest.h"
#include "parser.hh"
#include "ParserDriver.hh"
#include "type.h"
using namespace std;

const string RESOURCES_DIR = "../../tests/resources/";
const string COOL_PROGRAMS_DIR = RESOURCES_DIR + "CoolPrograms/";

stringstream makeTypeStringStreamFromReference(string fileName) {
    //generate the reference output
    string command = "cd " + RESOURCES_DIR + " && ./cool --type CoolPrograms/" + fileName;
    system(command.c_str());

    //read reference output into an ifstream
    string clLexFilePath = RESOURCES_DIR + "CoolPrograms/" + fileName + "-type";
    ifstream referenceLexerOutput(clLexFilePath);

    //make it a stringstream so we can compare
    stringstream s;
    s << referenceLexerOutput.rdbuf();
    return s;
}

TEST(TypeFull, helloworldType) {
    ParserDriver pdrv;
    const string localFile = "PA4example.cl";
    pdrv.file = COOL_PROGRAMS_DIR + localFile;

    _expr::printExprType = true;

    stringstream semanticAnalyzerOutput;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);
    _program* AST = (_program*) pdrv.buildSyntaxTree(rootIVAN);
    populateClassMap();
    printClassMap(semanticAnalyzerOutput);
    //printImplementationMap();
    semanticAnalyzerOutput << "implementation_map\n";
    populateParentMap();
    printParentMap(semanticAnalyzerOutput);
    //printAnnotatedAST();


    //TODO: DELETE WHEN DONE GENERATING TEST FILE
    ofstream out(COOL_PROGRAMS_DIR + "PA4example.cl-type2");
    out << semanticAnalyzerOutput.str();
    out.close();

    stringstream reference = makeTypeStringStreamFromReference(localFile);
    ASSERT_EQ(reference.str(), semanticAnalyzerOutput.str());
    globalEnv->reset();
}