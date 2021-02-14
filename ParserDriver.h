//
// Created by Ivan Rodriguez on 2/13/21.
//

#ifndef COOLCOMPILERPROJECT_PARSERDRIVER_H
#define COOLCOMPILERPROJECT_PARSERDRIVER_H
#include "lexer.yy.hpp"
/**
 * This class interfaces with flex and lemon. It carries thread-safe global data, as well as methods implementing
 * procedures associated with a production and methods managing the AST.
 */
 class ParserDriver;

void* ParseAlloc(void* (*allocProc)(size_t));
void Parse(void* parser, int token, const char* tokenInfo, ParserDriver* info);
void ParseFree(void* parser, void(*freeProc)(void*));

class ParserDriver {
private:
    yyscan_t scanner;
public:
    ParserDriver() {}
//    void parse(const string& fileName) {
//        yylex_init(&scanner);
//        string fileContents = "class Main1{}; class Main2{}; class Main3{};";
//        YY_BUFFER_STATE bufferState = yy_scan_string(fileContents.c_str(), scanner);
//
//        //set up the parser
//        void* parse = ParseAlloc(malloc);
//
//        int lexCode;
//        do {
//            lexCode = yylex(scanner);
//            if(lexCode == TYPE)
//        }
//
//    }
    ~ParserDriver() {}


    _program* ast;
    int ctr = 0;




    string id;
    void _program__classList(vector<_class*>*& CL) {
        ast = new _program(*CL);
    }
    void classList__classList_class(vector<_class*>*& CL1, vector<_class*>*& CL2, _class**& C) {
        CL1 = CL2;
        (*CL1).push_back(*C);
    }
    void classList(vector<_class*>*&  CL) {
        CL = new vector<_class*>;
    }
    void class__CLASS_TYPE_LBRACE_RBRACE_SEMI(_class**& C) {
        //cout << T << endl;
        C = new _class*;
        *C = new _class(id, "Object");
    }
};



#endif //COOLCOMPILERPROJECT_PARSERDRIVER_H
