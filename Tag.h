//
// Created by Ivan Rodriguez on 9/23/20.
//
#include <unordered_map>
#include <string>

#ifndef TAG_H
#define TAG_H

using namespace std;

enum Tags {
    AT = 1, CASE, CLASS, COLON, COMMA, DIVIDE, DOT, ELSE, EQUALS, ESAC, FALSE, FI, IDENTIFIER, IF, IN,
    INHERITS, INTEGER, ISVOID, LARROW, LBRACE, LE, LET, LOOP, LPAREN, LT, MINUS, NEW, NOT, OF, PLUS,
    POOL, RARROW, RBRACE, RPAREN, SEMI, STRING, THEN, TILDE, TIMES, TRUE, TYPE, WHILE
};

extern const unordered_map<int, string> tagMap;

//value could just be lexeme but then we'd have to convert string to int later
//typedef union{
//    string lexeme;
//    int intVal;
//    YYLVAL_T() {}
//} YYLVAL_T;

//extern YYLVAL_T yylval;

union YYLVAL_T {
    string lexeme;
    int intVal;
    YYLVAL_T() {}
    ~YYLVAL_T() {}
};

extern YYLVAL_T* yylval;


#endif