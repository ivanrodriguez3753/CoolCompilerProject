//
// Created by Ivan Rodriguez on 10/3/20.
//

#include <string>
#include <unordered_map>
#include "Tag.h"

using namespace std;

const unordered_map<int, string> tagMap {
        {
            {AT, "at"},
            {CASE, "case"},
            {CLASS, "class"},
            {COLON, "colon"},
            {COMMA, "comma"},
            {DIVIDE, "divide"},
            {DOT, "dot"},
            {ELSE, "else"},
            {EQUALS, "equals"},
            {ESAC, "esac"},
            {FALSE, "false"},
            {FI, "fi"},
            {IDENTIFIER, "identifier"},
            {IF, "if"},
            {IN, "in"},
            {INHERITS, "inherits"},
            {INTEGER, "integer"},
            {ISVOID, "isvoid"},
            {LARROW, "larrow"},
            {LBRACE, "lbrace"},
            {LE, "le"},
            {LET, "let"},
            {LOOP, "loop"},
            {LPAREN, "lparen"},
            {LT, "lt"},
            {MINUS, "minus"},
            {NEW, "new"},
            {NOT, "not"},
            {OF, "of"},
            {PLUS, "plus"},
            {POOL, "pool"},
            {RARROW, "rarrow"},
            {RBRACE, "rbrace"},
            {RPAREN, "rparen"},
            {SEMI, "semi"},
            {STRING, "string"},
            {THEN, "then"},
            {TILDE, "tilde"},
            {TIMES, "times"},
            {TRUE, "true"},
            {TYPE, "type"},
            {WHILE, "while"}
        }
};

YYLVAL_T* yylval = new YYLVAL_T;