//
// Created by Ivan Rodriguez on 9/24/20.
//
#include <string>
#include "Tag.h"
#include <vector>

using namespace std;

//Use this class for all Tokens that don't need attribute values (like keywords)
class Token {
public:
    const int tag; //this attribute identifies the token type/name
    const string lexeme;
    Token(int t, string l) : tag{t}, lexeme{l} {}
};