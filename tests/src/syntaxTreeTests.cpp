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
    _program* AST = pdrv.buildSyntaxTree(rootIVAN);
    parserOutput << *AST;

    stringstream reference = makeSyntaxTreeStringStreamFromReference(localFile);
    ASSERT_EQ(reference.str(), parserOutput.str());
    globalEnv->reset();
}

TEST(Fragments, classInherits) {
    ParserDriver pdrv;
    const string localFile = "classInherits.cl";
    pdrv.file = COOL_PROGRAMS_DIR + localFile;

    stringstream parserOutput;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);
    _program* AST = pdrv.buildSyntaxTree(rootIVAN);
    parserOutput << *AST;

    stringstream reference = makeSyntaxTreeStringStreamFromReference(localFile);
    ASSERT_EQ(reference.str(), parserOutput.str());
    globalEnv->reset();
}

TEST(Fragments, singleFieldInitSyntaxTree) {
    ParserDriver pdrv;
    const string localFile = "singleFieldInit.cl";
    pdrv.file = COOL_PROGRAMS_DIR + localFile;

    stringstream parserOutput;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);
    _program* AST = pdrv.buildSyntaxTree(rootIVAN);
    parserOutput << *AST;

    stringstream reference = makeSyntaxTreeStringStreamFromReference(localFile);
    ASSERT_EQ(reference.str(), parserOutput.str());
    globalEnv->reset();
}


TEST(Fragments, singleMethodNoFormalsSyntaxTree) {
    ParserDriver pdrv;
    const string localFile = "singleMethodNoFormals.cl";
    pdrv.file = COOL_PROGRAMS_DIR + localFile;

    stringstream parserOutput;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);
    _program* AST = pdrv.buildSyntaxTree(rootIVAN);
    parserOutput << *AST;

    stringstream reference = makeSyntaxTreeStringStreamFromReference(localFile);
    ASSERT_EQ(reference.str(), parserOutput.str());
    globalEnv->reset();
}


TEST(Fragments, singleMethodOneFormalsSyntaxTree) {
    ParserDriver pdrv;
    const string localFile = "singleMethodOneFormals.cl";
    pdrv.file = COOL_PROGRAMS_DIR + localFile;

    stringstream parserOutput;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);
    _program* AST = pdrv.buildSyntaxTree(rootIVAN);
    parserOutput << *AST;

    stringstream reference = makeSyntaxTreeStringStreamFromReference(localFile);
    ASSERT_EQ(reference.str(), parserOutput.str());
    globalEnv->reset();
}

TEST(Fragments, singleMethodTwoFormalsSyntaxTree) {
    ParserDriver pdrv;
    const string localFile = "singleMethodTwoFormals.cl";
    pdrv.file = COOL_PROGRAMS_DIR + localFile;

    stringstream parserOutput;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);
    _program* AST = pdrv.buildSyntaxTree(rootIVAN);
    parserOutput << *AST;

    stringstream reference = makeSyntaxTreeStringStreamFromReference(localFile);
    ASSERT_EQ(reference.str(), parserOutput.str());
    globalEnv->reset();
}

TEST(Fragments, singleMethodFiveFormalsSyntaxTree) {
    ParserDriver pdrv;
    const string localFile = "singleMethodFiveFormals.cl";
    pdrv.file = COOL_PROGRAMS_DIR + localFile;

    stringstream parserOutput;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);
    _program* AST = pdrv.buildSyntaxTree(rootIVAN);
    parserOutput << *AST;

    stringstream reference = makeSyntaxTreeStringStreamFromReference(localFile);
    ASSERT_EQ(reference.str(), parserOutput.str());
    globalEnv->reset();
}

TEST(Fragments, allDispatchesNoArgumentsSyntaxTree) {
    ParserDriver pdrv;
    const string localFile = "allDispatchesNoArguments.cl";
    pdrv.file = COOL_PROGRAMS_DIR + localFile;

    stringstream parserOutput;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);
    _program* AST = pdrv.buildSyntaxTree(rootIVAN);
    parserOutput << *AST;

    stringstream reference = makeSyntaxTreeStringStreamFromReference(localFile);
    ASSERT_EQ(reference.str(), parserOutput.str());
    globalEnv->reset();
}

TEST(Fragments, allDispatchesOneArgumentsSyntaxTree) {
    ParserDriver pdrv;
    const string localFile = "allDispatchesOneArguments.cl";
    pdrv.file = COOL_PROGRAMS_DIR + localFile;

    stringstream parserOutput;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);
    _program* AST = pdrv.buildSyntaxTree(rootIVAN);
    parserOutput << *AST;

    stringstream reference = makeSyntaxTreeStringStreamFromReference(localFile);
    ASSERT_EQ(reference.str(), parserOutput.str());
    globalEnv->reset();
}

TEST(Fragments, allDispatchesTwoArgumentsSyntaxTree) {
    ParserDriver pdrv;
    const string localFile = "allDispatchesTwoArguments.cl";
    pdrv.file = COOL_PROGRAMS_DIR + localFile;

    stringstream parserOutput;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);
    _program* AST = pdrv.buildSyntaxTree(rootIVAN);
    parserOutput << *AST;

    stringstream reference = makeSyntaxTreeStringStreamFromReference(localFile);
    ASSERT_EQ(reference.str(), parserOutput.str());
    globalEnv->reset();
}

TEST(Fragments, allDispatchesFiveArgumentsSyntaxTree) {
    ParserDriver pdrv;
    const string localFile = "allDispatchesFiveArguments.cl";
    pdrv.file = COOL_PROGRAMS_DIR + localFile;

    stringstream parserOutput;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);
    _program* AST = pdrv.buildSyntaxTree(rootIVAN);
    parserOutput << *AST;

    stringstream reference = makeSyntaxTreeStringStreamFromReference(localFile);
    ASSERT_EQ(reference.str(), parserOutput.str());
    globalEnv->reset();
}

TEST(Fragments, ifWhileExpressionsSyntaxTree) {
    ParserDriver pdrv;
    const string localFile = "ifWhileExpressions.cl";
    pdrv.file = COOL_PROGRAMS_DIR + localFile;

    stringstream parserOutput;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);
    _program* AST = pdrv.buildSyntaxTree(rootIVAN);
    parserOutput << *AST;

    stringstream reference = makeSyntaxTreeStringStreamFromReference(localFile);
    ASSERT_EQ(reference.str(), parserOutput.str());
    globalEnv->reset();
}

TEST(Fragments, assignExprSyntaxTree) {
    ParserDriver pdrv;
    const string localFile = "assignExpr.cl";
    pdrv.file = COOL_PROGRAMS_DIR + localFile;

    stringstream parserOutput;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);
    _program* AST = pdrv.buildSyntaxTree(rootIVAN);
    parserOutput << *AST;

    stringstream reference = makeSyntaxTreeStringStreamFromReference(localFile);
    ASSERT_EQ(reference.str(), parserOutput.str());
    globalEnv->reset();
}

TEST(Fragments, blockExprOneExpressionsSyntaxTree) {
    ParserDriver pdrv;
    const string localFile = "blockExprOneExpressions.cl";
    pdrv.file = COOL_PROGRAMS_DIR + localFile;

    stringstream parserOutput;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);
    _program* AST = pdrv.buildSyntaxTree(rootIVAN);
    parserOutput << *AST;

    stringstream reference = makeSyntaxTreeStringStreamFromReference(localFile);
    ASSERT_EQ(reference.str(), parserOutput.str());
    globalEnv->reset();
}

TEST(Fragments, blockExprTwoExpressionsSyntaxTree) {
    ParserDriver pdrv;
    const string localFile = "blockExprTwoExpressions.cl";
    pdrv.file = COOL_PROGRAMS_DIR + localFile;

    stringstream parserOutput;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);
    _program* AST = pdrv.buildSyntaxTree(rootIVAN);
    parserOutput << *AST;

    stringstream reference = makeSyntaxTreeStringStreamFromReference(localFile);
    ASSERT_EQ(reference.str(), parserOutput.str());
    globalEnv->reset();
}

TEST(Fragments, blockExprFiveExpressionsSyntaxTree) {
    ParserDriver pdrv;
    const string localFile = "blockExprFiveExpressions.cl";
    pdrv.file = COOL_PROGRAMS_DIR + localFile;

    stringstream parserOutput;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);
    _program* AST = pdrv.buildSyntaxTree(rootIVAN);
    parserOutput << *AST;

    stringstream reference = makeSyntaxTreeStringStreamFromReference(localFile);
    ASSERT_EQ(reference.str(), parserOutput.str());
    globalEnv->reset();
}

TEST(Fragments, newExprSyntaxTree) {
    ParserDriver pdrv;
    const string localFile = "newExpr.cl";
    pdrv.file = COOL_PROGRAMS_DIR + localFile;

    stringstream parserOutput;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);
    _program* AST = pdrv.buildSyntaxTree(rootIVAN);
    parserOutput << *AST;

    stringstream reference = makeSyntaxTreeStringStreamFromReference(localFile);
    ASSERT_EQ(reference.str(), parserOutput.str());
    globalEnv->reset();
}

TEST(Fragments, isvoidExprSyntaxTree) {
    ParserDriver pdrv;
    const string localFile = "isvoidExpr.cl";
    pdrv.file = COOL_PROGRAMS_DIR + localFile;

    stringstream parserOutput;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);
    _program* AST = pdrv.buildSyntaxTree(rootIVAN);
    parserOutput << *AST;

    stringstream reference = makeSyntaxTreeStringStreamFromReference(localFile);
    ASSERT_EQ(reference.str(), parserOutput.str());
    globalEnv->reset();
}

TEST(Fragments, integerPlusMinusTimesDivideExpressionsSyntaxTree) {
    ParserDriver pdrv;
    const string localFile = "integerPlusMinusTimesDivideExpressions.cl";
    pdrv.file = COOL_PROGRAMS_DIR + localFile;

    stringstream parserOutput;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);
    _program* AST = pdrv.buildSyntaxTree(rootIVAN);
    parserOutput << *AST;

    stringstream reference = makeSyntaxTreeStringStreamFromReference(localFile);
    ASSERT_EQ(reference.str(), parserOutput.str());
    globalEnv->reset();
}

TEST(Fragments, relationalExprSyntaxTree) {
    ParserDriver pdrv;
    const string localFile = "relationalExpr.cl";
    pdrv.file = COOL_PROGRAMS_DIR + localFile;

    stringstream parserOutput;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);
    _program* AST = pdrv.buildSyntaxTree(rootIVAN);
    parserOutput << *AST;

    stringstream reference = makeSyntaxTreeStringStreamFromReference(localFile);
    ASSERT_EQ(reference.str(), parserOutput.str());
    globalEnv->reset();
}

TEST(Fragments, notNegateExprSyntaxTree) {
    ParserDriver pdrv;
    const string localFile = "notNegateExpr.cl";
    pdrv.file = COOL_PROGRAMS_DIR + localFile;

    stringstream parserOutput;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);
    _program* AST = pdrv.buildSyntaxTree(rootIVAN);
    parserOutput << *AST;

    stringstream reference = makeSyntaxTreeStringStreamFromReference(localFile);
    ASSERT_EQ(reference.str(), parserOutput.str());
    globalEnv->reset();
}

TEST(Fragments, parenthesizedExprSyntaxTree) {
    ParserDriver pdrv;
    const string localFile = "parenthesizedExpr.cl";
    pdrv.file = COOL_PROGRAMS_DIR + localFile;

    stringstream parserOutput;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);
    _program* AST = pdrv.buildSyntaxTree(rootIVAN);
    parserOutput << *AST;

    stringstream reference = makeSyntaxTreeStringStreamFromReference(localFile);
    ASSERT_EQ(reference.str(), parserOutput.str());
    globalEnv->reset();
}

TEST(Fragments, stringExprSyntaxTree) {
    ParserDriver pdrv;
    const string localFile = "stringExpr.cl";
    pdrv.file = COOL_PROGRAMS_DIR + localFile;

    stringstream parserOutput;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);
    _program* AST = pdrv.buildSyntaxTree(rootIVAN);
    parserOutput << *AST;

    stringstream reference = makeSyntaxTreeStringStreamFromReference(localFile);
    ASSERT_EQ(reference.str(), parserOutput.str());
    globalEnv->reset();
}

TEST(Fragments, identifierExprSyntaxTree) {
    ParserDriver pdrv;
    const string localFile = "identifierExpr.cl";
    pdrv.file = COOL_PROGRAMS_DIR + localFile;

    stringstream parserOutput;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);
    _program* AST = pdrv.buildSyntaxTree(rootIVAN);
    parserOutput << *AST;

    stringstream reference = makeSyntaxTreeStringStreamFromReference(localFile);
    ASSERT_EQ(reference.str(), parserOutput.str());
    globalEnv->reset();
}

TEST(Fragments, booleanExprSyntaxTree) {
    ParserDriver pdrv;
    const string localFile = "booleanExpr.cl";
    pdrv.file = COOL_PROGRAMS_DIR + localFile;

    stringstream parserOutput;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);
    _program* AST = pdrv.buildSyntaxTree(rootIVAN);
    parserOutput << *AST;

    stringstream reference = makeSyntaxTreeStringStreamFromReference(localFile);
    ASSERT_EQ(reference.str(), parserOutput.str());
    globalEnv->reset();
}

TEST(Fragments, letExprOneBindingNoInitSyntaxTree) {
    ParserDriver pdrv;
    const string localFile = "letExprOneBindingNoInit.cl";
    pdrv.file = COOL_PROGRAMS_DIR + localFile;

    stringstream parserOutput;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);
    _program* AST = pdrv.buildSyntaxTree(rootIVAN);
    parserOutput << *AST;

    stringstream reference = makeSyntaxTreeStringStreamFromReference(localFile);
    ASSERT_EQ(reference.str(), parserOutput.str());
    globalEnv->reset();
}

TEST(Fragments, letExprOneBindingYesInitSyntaxTree) {
    ParserDriver pdrv;
    const string localFile = "letExprOneBindingYesInit.cl";
    pdrv.file = COOL_PROGRAMS_DIR + localFile;

    stringstream parserOutput;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);
    _program* AST = pdrv.buildSyntaxTree(rootIVAN);
    parserOutput << *AST;

    stringstream reference = makeSyntaxTreeStringStreamFromReference(localFile);
    ASSERT_EQ(reference.str(), parserOutput.str());
    globalEnv->reset();
}

TEST(Fragments, letExprMultipleBindingMixedInitSyntaxTree) {
    ParserDriver pdrv;
    const string localFile = "letExprMultipleBindingMixedInit.cl";
    pdrv.file = COOL_PROGRAMS_DIR + localFile;

    stringstream parserOutput;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);
    _program* AST = pdrv.buildSyntaxTree(rootIVAN);
    parserOutput << *AST;

    stringstream reference = makeSyntaxTreeStringStreamFromReference(localFile);
    ASSERT_EQ(reference.str(), parserOutput.str());
    globalEnv->reset();
}

TEST(Fragments, caseExprOneCaseSyntaxTree) {
    ParserDriver pdrv;
    const string localFile = "caseExprOneCase.cl";
    pdrv.file = COOL_PROGRAMS_DIR + localFile;

    stringstream parserOutput;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);
    _program* AST = pdrv.buildSyntaxTree(rootIVAN);
    parserOutput << *AST;

    stringstream reference = makeSyntaxTreeStringStreamFromReference(localFile);
    ASSERT_EQ(reference.str(), parserOutput.str());
    globalEnv->reset();
}

TEST(Fragments, caseExprManyCaseSyntaxTree) {
    ParserDriver pdrv;
    const string localFile = "caseExprManyCase.cl";
    pdrv.file = COOL_PROGRAMS_DIR + localFile;

    stringstream parserOutput;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);
    _program* AST = pdrv.buildSyntaxTree(rootIVAN);
    parserOutput << *AST;

    stringstream reference = makeSyntaxTreeStringStreamFromReference(localFile);
    ASSERT_EQ(reference.str(), parserOutput.str());
    globalEnv->reset();
}

TEST(AbstractParseFull, arithSyntaxTree) {
    ParserDriver pdrv;
    const string localFile = "arith.cl";
    pdrv.file = COOL_PROGRAMS_DIR + localFile;

    stringstream parserOutput;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);
    _program* AST = pdrv.buildSyntaxTree(rootIVAN);
    parserOutput << *AST;

    stringstream reference = makeSyntaxTreeStringStreamFromReference(localFile);
    ASSERT_EQ(reference.str(), parserOutput.str());
    globalEnv->reset();
}

TEST(AbstractParseFull, atoiSyntaxTree) {
    ParserDriver pdrv;
    const string localFile = "atoi.cl";
    pdrv.file = COOL_PROGRAMS_DIR + localFile;

    stringstream parserOutput;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);
    _program* AST = pdrv.buildSyntaxTree(rootIVAN);
    parserOutput << *AST;

    stringstream reference = makeSyntaxTreeStringStreamFromReference(localFile);
    ASSERT_EQ(reference.str(), parserOutput.str());
    globalEnv->reset();
}

TEST(AbstractParseFull, cellsSyntaxTree) {
    ParserDriver pdrv;
    const string localFile = "cells.cl";
    pdrv.file = COOL_PROGRAMS_DIR + localFile;

    stringstream parserOutput;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);
    _program* AST = pdrv.buildSyntaxTree(rootIVAN);
    parserOutput << *AST;

    stringstream reference = makeSyntaxTreeStringStreamFromReference(localFile);
    ASSERT_EQ(reference.str(), parserOutput.str());
    globalEnv->reset();
}



TEST(AbstractParseFull, hsSyntaxTree) {
    ParserDriver pdrv;
    const string localFile = "hs.cl";
    pdrv.file = COOL_PROGRAMS_DIR + localFile;

    stringstream parserOutput;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);
    _program* AST = pdrv.buildSyntaxTree(rootIVAN);
    parserOutput << *AST;

    stringstream reference = makeSyntaxTreeStringStreamFromReference(localFile);
    ASSERT_EQ(reference.str(), parserOutput.str());
    globalEnv->reset();
}

TEST(AbstractParseFull, listSyntaxTree) {
    ParserDriver pdrv;
    const string localFile = "list.cl";
    pdrv.file = COOL_PROGRAMS_DIR + localFile;

    stringstream parserOutput;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);
    _program* AST = pdrv.buildSyntaxTree(rootIVAN);
    parserOutput << *AST;

    stringstream reference = makeSyntaxTreeStringStreamFromReference(localFile);
    ASSERT_EQ(reference.str(), parserOutput.str());
    globalEnv->reset();
}

TEST(AbstractParseFull, newcomplexSyntaxTree) {
    ParserDriver pdrv;
    const string localFile = "new-complex.cl";
    pdrv.file = COOL_PROGRAMS_DIR + localFile;

    stringstream parserOutput;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);
    _program* AST = pdrv.buildSyntaxTree(rootIVAN);
    parserOutput << *AST;

    stringstream reference = makeSyntaxTreeStringStreamFromReference(localFile);
    ASSERT_EQ(reference.str(), parserOutput.str());
    globalEnv->reset();
}

TEST(AbstractParseFull, primesSyntaxTree) {
    ParserDriver pdrv;
    const string localFile = "primes.cl";
    pdrv.file = COOL_PROGRAMS_DIR + localFile;

    stringstream parserOutput;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);
    _program* AST = pdrv.buildSyntaxTree(rootIVAN);
    parserOutput << *AST;

    stringstream reference = makeSyntaxTreeStringStreamFromReference(localFile);
    ASSERT_EQ(reference.str(), parserOutput.str());
    globalEnv->reset();
}

TEST(AbstractParseFull, printcoolSyntaxTree) {
    ParserDriver pdrv;
    const string localFile = "print-cool.cl";
    pdrv.file = COOL_PROGRAMS_DIR + localFile;

    stringstream parserOutput;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);
    _program* AST = pdrv.buildSyntaxTree(rootIVAN);
    parserOutput << *AST;

    stringstream reference = makeSyntaxTreeStringStreamFromReference(localFile);
    ASSERT_EQ(reference.str(), parserOutput.str());
    globalEnv->reset();
}

TEST(AbstractParseFull, sortlistSyntaxTree) {
    ParserDriver pdrv;
    const string localFile = "sort-list.cl";
    pdrv.file = COOL_PROGRAMS_DIR + localFile;

    stringstream parserOutput;
    pdrv.parse(COOL_PROGRAMS_DIR + localFile);
    _program* AST = pdrv.buildSyntaxTree(rootIVAN);
    parserOutput << *AST;

    stringstream reference = makeSyntaxTreeStringStreamFromReference(localFile);
    ASSERT_EQ(reference.str(), parserOutput.str());
    globalEnv->reset();
}