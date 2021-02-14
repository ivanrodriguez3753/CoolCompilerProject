%include
{
 #include <cassert>
 #include <iostream>

 #include "ast.h"
 #include "ParserDriver.h"

 #define YYSTYPE const char
 using namespace std;

}

%syntax_error
{

}

%token_type {const char*}
%type class {_class**}
%type classList {vector<_class*>*}
%extra_argument {ParserDriver* drv}

program ::= classList(CL) .
{
    drv->_program__classList(CL);
}

program ::= ELSE FALSE IF FI IN INHERITS ISVOID LET LOOP POOL THEN WHILE CASE ESAC NEW OF NOT TRUE ID COLON LPAREN RPAREN LARROW DOT COMMA AT PLUS MINUS TIMES DIVIDE NEG LT LE RARROW EQ .
{

}

classList(CL1) ::= classList(CL2) class(C) .
{
    drv->classList__classList_class(CL1, CL2, C);
}

classList(CL) ::= .
{
    drv->classList(CL);
}

class(C) ::= CLASS TYPE LBRACE RBRACE SEMI .
{
    drv->class__CLASS_TYPE_LBRACE_RBRACE_SEMI(C);
}