#include <cstdlib>
#include <iostream>
#include <string>
#include "parse.hpp"
#include "lexer.yy.hpp"
#include "ast.h"
#include "omp.h"
#include "ParserDriver.h"


using namespace std;

 
void* ParseAlloc(void* (*allocProc)(size_t));
void Parse(void* parser, int token, const char* tokenInfo, ParserDriver* info);
void ParseFree(void* parser, void(*freeProc)(void*));
 
void parse(const string& commandLine) {
    // Set up the scanner
    ParserDriver drv;
    yyscan_t scanner;
    yylex_init(&scanner);
    YY_BUFFER_STATE bufferState = yy_scan_string(commandLine.c_str(), scanner);
 
    // Set up the parser
    void* parse = ParseAlloc(malloc);
 
    int lexCode;
    do {
        lexCode = yylex(scanner);
        if(lexCode == TYPE) {
            drv.id = yyget_text(scanner);
        }
        Parse(parse, lexCode, NULL, &drv);
    }
    while (lexCode > 0);

    cout << "traversing class list\n";
    for(_class* klass : drv.ast->classList) {
        cout << klass->id << endl;
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

    (parse("class Main {}; "
          "class Main2 {}; "
          "class Main3 {};")
    );
    //parse("class Main {}; class Main2 {}; class Main3 {};");

//    cout << omp_get_num_threads() << endl;

#pragma omp parallel num_threads(1)
{
    printf("hello world!\n");
}
    return 0;
}
