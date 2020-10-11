#include <fstream>
#include "lex.yy.h"
#include "Tag.h"
#include "gtest/gtest.h"
using namespace std;

const string RESOURCES_DIR = "../../tests/resources/";
const string COOL_PROGRAMS_DIR = RESOURCES_DIR + "CoolPrograms/";

stringstream makeStringStreamFromReference(string fileName) {
    //generate the reference output
    string command = "cd " + RESOURCES_DIR + " && ./cool --lex CoolPrograms/" + fileName;
    system(command.c_str());

    //read reference output into an ifstream
    string clLexFilePath = RESOURCES_DIR + "CoolPrograms/" + fileName + "-lex";
    ifstream referenceLexerOutput(clLexFilePath);

    //make it a stringstream so we can compare
    stringstream s;
    s << referenceLexerOutput.rdbuf();
    return s;
}

/**
 * TODO: yylineno is a global, so all the tests pass if run individually but only the first one passes if I run them all.
 * TODO: The workaround for now is to reset yylineno to 1 for every test
 * @param lexerOutput
 */
void lexInput(stringstream& lexerOutput) {
    yylineno = 1;
    int token;
    while( (token = yylex()) ) {
        if(token != 999) {
            lexerOutput << yylineno << endl << tagMap.at(token) << endl;
            if(token == IDENTIFIER || token == STRING || token == TYPE || token == INTEGER) {
                lexerOutput << yylval->lexeme << endl;
            }
        }
    }
}

TEST(LexerFull, arith) {
    string fileName = COOL_PROGRAMS_DIR + "arith.cl";
    FILE* inputFile = fopen(fileName.c_str(), "r");
    yyin = inputFile;

    stringstream lexerOutput;
    lexInput(lexerOutput);

    stringstream reference = makeStringStreamFromReference("arith.cl");
    ASSERT_EQ(reference.str(), lexerOutput.str());
}

TEST(LexerFull, atoi) {
    string fileName = COOL_PROGRAMS_DIR + "atoi.cl";
    FILE* inputFile = fopen(fileName.c_str(), "r");
    yyin = inputFile;

    stringstream lexerOutput;
    lexInput(lexerOutput);

    stringstream reference = makeStringStreamFromReference("atoi.cl");
    ASSERT_EQ(reference.str(), lexerOutput.str());
}

TEST(LexerFull, cells) {
    string fileName = COOL_PROGRAMS_DIR + "cells.cl";
    FILE* inputFile = fopen(fileName.c_str(), "r");
    yyin = inputFile;

    stringstream lexerOutput;
    lexInput(lexerOutput);

    stringstream reference = makeStringStreamFromReference("cells.cl");
    ASSERT_EQ(reference.str(), lexerOutput.str());
}

TEST(LexerFull, helloworld) {
    string fileName = COOL_PROGRAMS_DIR + "hello-world.cl";
    FILE* inputFile = fopen(fileName.c_str(), "r");
    yyin = inputFile;

    stringstream lexerOutput;
    lexInput(lexerOutput);

    stringstream reference = makeStringStreamFromReference("hello-world.cl");

    //compare the ifstream reference output to the stringstream lexerOutput
    ASSERT_EQ(reference.str(), lexerOutput.str());
}

TEST(LexerFull, hs) {
    string fileName = COOL_PROGRAMS_DIR + "hs.cl";
    FILE* inputFile = fopen(fileName.c_str(), "r");
    yyin = inputFile;

    stringstream lexerOutput;
    lexInput(lexerOutput);

    stringstream reference = makeStringStreamFromReference("hs.cl");
    ASSERT_EQ(reference.str(), lexerOutput.str());
}

TEST(LexerFull, list) {
    string fileName = COOL_PROGRAMS_DIR + "list.cl";
    FILE* inputFile = fopen(fileName.c_str(), "r");
    yyin = inputFile;

    stringstream lexerOutput;
    lexInput(lexerOutput);

    stringstream reference = makeStringStreamFromReference("list.cl");
    ASSERT_EQ(reference.str(), lexerOutput.str());
}

TEST(LexerFull, newcomplex) {
    string fileName = COOL_PROGRAMS_DIR + "new-complex.cl";
    FILE* inputFile = fopen(fileName.c_str(), "r");
    yyin = inputFile;

    stringstream lexerOutput;
    lexInput(lexerOutput);

    stringstream reference = makeStringStreamFromReference("new-complex.cl");
    ASSERT_EQ(reference.str(), lexerOutput.str());
}

TEST(LexerFull, primes) {
    string fileName = COOL_PROGRAMS_DIR + "primes.cl";
    FILE* inputFile = fopen(fileName.c_str(), "r");
    yyin = inputFile;

    stringstream lexerOutput;
    lexInput(lexerOutput);

    stringstream reference = makeStringStreamFromReference("primes.cl");
    ASSERT_EQ(reference.str(), lexerOutput.str());
}

TEST(LexerFull, printcool) {
    string fileName = COOL_PROGRAMS_DIR + "print-cool.cl";
    FILE* inputFile = fopen(fileName.c_str(), "r");
    yyin = inputFile;

    stringstream lexerOutput;
    lexInput(lexerOutput);

    stringstream reference = makeStringStreamFromReference("print-cool.cl");
    ASSERT_EQ(reference.str(), lexerOutput.str());
}

TEST(LexerFull, sortlist) {
    string fileName = COOL_PROGRAMS_DIR + "sort-list.cl";
    FILE* inputFile = fopen(fileName.c_str(), "r");
    yyin = inputFile;

    stringstream lexerOutput;
    lexInput(lexerOutput);

    stringstream reference = makeStringStreamFromReference("sort-list.cl");
    ASSERT_EQ(reference.str(), lexerOutput.str());
}