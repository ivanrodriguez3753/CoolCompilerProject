/*
 * This file provides control mechanisms for the parser, which uses the lexer. Follows the tutorial for the
 * complete C++ parser in Chapter10 of the bison documentation
 */

#ifndef PARSERDRIVER_H
#define PARSERDRIVER_H


#include <map>
#include <string>
#include "ast.h"
#include "parser.hpp"
#include "environment.h"


//give Flex the prototype of yylex we want...
#define YY_DECL yy::parser::symbol_type yylex(ParserDriver& drv)
//...and declare it for the parser's sake
YY_DECL;

using namespace std;

class ParserDriver {
public:
    int encountered = 0;

    _program* ast;

    ParserDriver();
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

    void printHi() {cout << "hiiii\n";}
    friend class _node;
private:
};

extern map<string, string> tokenReqTranslation;

#endif //PARSERDRIVER_H
