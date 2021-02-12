%include
{
 #include <cassert>
 #include <iostream>

 #include "syntaxTreeNodes.h"
 using namespace std;
}

%syntax_error
{
  *valid = false;
}

%token_type {const char*}
%type program {_program*}
%extra_argument {bool* valid}

program ::= classList .
{
    cout << "program rule\n";
    new _program;

}

classList ::= classList class .
{
    cout << "append class node\n";
}

classList ::= .
{
    cout << "spawn new classList node\n";
}

class ::= CLASS TYPE LBRACE RBRACE SEMI .
{
    cout << "found a class!\n";
}