#include <cstdlib>
#include <iostream>
#include <string>
#include "parse.hpp"
#include "lexer.yy.hpp"
#include "ast.h"
#include "omp.h"

using namespace std;
 
void* ParseAlloc(void* (*allocProc)(size_t));
void Parse(void* parser, int token, const char* tokenInfo, bool* valid);
void ParseFree(void* parser, void(*freeProc)(void*));
 
void parse(const string& commandLine) {
    // Set up the scanner
    yyscan_t scanner;
    yylex_init(&scanner);
    YY_BUFFER_STATE bufferState = yy_scan_string(commandLine.c_str(), scanner);
 
    // Set up the parser
    void* parse = ParseAlloc(malloc);
 
    int lexCode;
    bool validParse = true;
    do {
        lexCode = yylex(scanner);
        Parse(parse, lexCode, NULL, &validParse);
    }
    while (lexCode > 0 && validParse);
 
    if (-1 == lexCode) {
        cerr << "The scanner encountered an error.\n";
    }

    if (!validParse) {
        cerr << "The parser encountered an error.\n";
    }
 
    // Cleanup the scanner and parser
    yy_delete_buffer(bufferState, scanner);
    yylex_destroy(scanner);
    ParseFree(parse, free);
}
 
int main() {
//    int a = 3;
//    a + 2;
//    string commandLine;
//    while (getline(cin, commandLine)) {
//        parse(commandLine);
//    }

    parse("class Main {}; class Main2 {}; class Main3 {};");

//    cout << omp_get_num_threads() << endl;

#pragma omp parallel num_threads(1)
{
    printf("hello world!\n");
}
    return 0;
}
