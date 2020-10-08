#include <fstream>
#include "lex.yy.h"
#include "Tag.h"
#include "gtest/gtest.h"
using namespace std;

string RESOURCES_DIR = "../../tests/resources/";

TEST(LexerFull, atoi) {
    string fileName = RESOURCES_DIR + "CoolPrograms/" + "hello-world.cl";
    FILE* inputFile = fopen(fileName.c_str(), "r");
    yyin = inputFile;

    stringstream lexerOutput;

    //fill in the lexerOutput stream
    int token;
    while( (token = yylex()) ) {
        if(token != 999) {
            lexerOutput << yylineno << endl << tagMap.at(token) << endl;
            if(token == IDENTIFIER || token == STRING || token == TYPE || token == INTEGER) {
                lexerOutput << yylval->lexeme << endl;
            }
        }
    }

    //generate the reference output
    string command = "cd ../../tests/resources/ && ./cool --lex CoolPrograms/hello-world.cl";
    system(command.c_str());

    //read reference output into an ifstream
    string clLexFilePath = RESOURCES_DIR + "CoolPrograms/hello-world.cl-lex";
    ifstream referenceLexerOutput(clLexFilePath);

    //make it a stringstream so we can compare
    stringstream s;
    s << referenceLexerOutput.rdbuf();

    //compare the ifstream reference output to the stringstream lexerOutput
    ASSERT_EQ(s.str(), lexerOutput.str());
}