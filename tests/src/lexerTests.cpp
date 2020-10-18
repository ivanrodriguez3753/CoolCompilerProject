#include <fstream>
#include "gtest/gtest.h"
#include "parser.hh"
#include "ParserDriver.hh"
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


void lexInput(ParserDriver& pdrv, stringstream& lexerOutput) {
    pdrv.scan_begin();
    bool eof = false;
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

TEST(LexerFull, arith) {
    ParserDriver pdrv;
    const string localFile = "arith.cl";
    pdrv.file = COOL_PROGRAMS_DIR + localFile;

    stringstream lexerOutput;
    lexInput(pdrv, lexerOutput);

    stringstream reference = makeStringStreamFromReference(localFile);
    ASSERT_EQ(reference.str(), lexerOutput.str());
}

TEST(LexerFull, atoi) {
    ParserDriver pdrv;
    const string localFile = "atoi.cl";
    pdrv.file = COOL_PROGRAMS_DIR + localFile;

    stringstream lexerOutput;
    lexInput(pdrv, lexerOutput);

    stringstream reference = makeStringStreamFromReference(localFile);
    ASSERT_EQ(reference.str(), lexerOutput.str());
}

TEST(LexerFull, cells) {
    ParserDriver pdrv;
    const string localFile = "cells.cl";
    pdrv.file = COOL_PROGRAMS_DIR + localFile;

    stringstream lexerOutput;
    lexInput(pdrv, lexerOutput);

    stringstream reference = makeStringStreamFromReference(localFile);
    ASSERT_EQ(reference.str(), lexerOutput.str());
}

TEST(LexerFull, helloworld) {
    ParserDriver pdrv;
    const string localFile = "hello-world.cl";
    pdrv.file = COOL_PROGRAMS_DIR + localFile;

    stringstream lexerOutput;
    lexInput(pdrv, lexerOutput);

    stringstream reference = makeStringStreamFromReference(localFile);
    ASSERT_EQ(reference.str(), lexerOutput.str());
}

TEST(LexerFull, hs) {
    ParserDriver pdrv;
    const string localFile = "hs.cl";
    pdrv.file = COOL_PROGRAMS_DIR + localFile;

    stringstream lexerOutput;
    lexInput(pdrv, lexerOutput);

    stringstream reference = makeStringStreamFromReference(localFile);
    ASSERT_EQ(reference.str(), lexerOutput.str());
}

TEST(LexerFull, list) {
    ParserDriver pdrv;
    const string localFile = "list.cl";
    pdrv.file = COOL_PROGRAMS_DIR + localFile;

    stringstream lexerOutput;
    lexInput(pdrv, lexerOutput);

    stringstream reference = makeStringStreamFromReference(localFile);
    ASSERT_EQ(reference.str(), lexerOutput.str());
}

TEST(LexerFull, newcomplex) {
    ParserDriver pdrv;
    const string localFile = "new-complex.cl";
    pdrv.file = COOL_PROGRAMS_DIR + localFile;

    stringstream lexerOutput;
    lexInput(pdrv, lexerOutput);

    stringstream reference = makeStringStreamFromReference(localFile);
    ASSERT_EQ(reference.str(), lexerOutput.str());
}

TEST(LexerFull, primes) {
    ParserDriver pdrv;
    const string localFile = "primes.cl";
    pdrv.file = COOL_PROGRAMS_DIR + localFile;

    stringstream lexerOutput;
    lexInput(pdrv, lexerOutput);

    stringstream reference = makeStringStreamFromReference(localFile);
    ASSERT_EQ(reference.str(), lexerOutput.str());
}

TEST(LexerFull, printcool) {
    ParserDriver pdrv;
    const string localFile = "print-cool.cl";
    pdrv.file = COOL_PROGRAMS_DIR + localFile;

    stringstream lexerOutput;
    lexInput(pdrv, lexerOutput);

    stringstream reference = makeStringStreamFromReference(localFile);
    ASSERT_EQ(reference.str(), lexerOutput.str());
}

TEST(LexerFull, sortlist) {
    ParserDriver pdrv;
    const string localFile = "sort-list.cl";
    pdrv.file = COOL_PROGRAMS_DIR + localFile;

    stringstream lexerOutput;
    lexInput(pdrv, lexerOutput);

    stringstream reference = makeStringStreamFromReference(localFile);
    ASSERT_EQ(reference.str(), lexerOutput.str());
}