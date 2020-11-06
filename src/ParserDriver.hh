/*
 * This file provides control mechanisms for the parser, which uses the lexer. Follows the tutorial for the
 * complete C++ parser in Chapter10 of the bison documentation
 */

#ifndef PARSERDRIVER_H
#define PARSERDRIVER_H


#include <map>
#include <string>
#include "parser.hh"
#include "syntaxTreeNodes.h"
#include <Environment.h>


//give Flex the prototype of yylex we want...
#define YY_DECL yy::parser::symbol_type yylex(ParserDriver& drv)
//...and declare it for the parser's sake
YY_DECL;

using namespace std;

class ParserDriver {
public:
    ParserDriver();
    std::map<std::string, int> variables;
    int result;

    //main routine is calling the parser
    int parse(const std::string& f);

    //name of file being parsed
    std::string file;

    bool trace_parsing;

    //handle scanner
    void scan_begin();
    void scan_end();
    bool trace_scanning;

    //Token's location used by scanner
    yy::location location;

    void postorderTraversal(ostream& out);
    void prettyPrintTree(ostream& out);

    _program* buildSyntaxTree(programNode* root);
    _node* buildSyntaxNode(node* current);

    /**
     * Keep a pointer to the current env/symtable. Initialize with globalEnv
     */

    friend class _node;
private:
    void postorderRecurs(node* current, ostream& out);
    void prettyPrintRecurs(node* current, const string& prefix, ostream& out);
};

extern map<string, string> tokenReqTranslation;

#endif //PARSERDRIVER_H
