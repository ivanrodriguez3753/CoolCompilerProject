%include
{
// #include <iostream>

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
%type featureList {pair<vector<_attr*>, vector<_method*>>*}
%type formalsList {vector<_formal*>*}
%type moreFormalsList {vector<_formal*>*}
%type formal {_formal**}
%type firstFormal {_formal**}
%type id {string*}
%type type {string*}
%type optionalInh{string*}
%type optInit{_expr**}

%type feature {featureUnion}
%type attr {_attr**}
%type method {_method**}
%type expr {_expr**}
%extra_argument {ParserDriver* drv}

program ::= classList(CL) .
{
    drv->program__classList(CL);
}

program ::= ELSE FALSE IF FI IN INHERITS ISVOID LET LOOP POOL THEN WHILE CASE ESAC NEW OF NOT TRUE COLON LPAREN RPAREN LARROW DOT COMMA AT PLUS MINUS TIMES DIVIDE NEG LT LE RARROW EQ INT ID.
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

class(C) ::= CLASS type(T) optionalInh(SUPER_T) LBRACE featureList(FL) RBRACE SEMI .
{
    drv->class__CLASS_type_optionalInh_LBRACE_featureList_RBRACE_SEMI(C, T, SUPER_T, FL);
}

optionalInh(T) ::= .
{
    drv->optionalInh(T);
}

optionalInh(T1) ::= INHERITS type(T2) .
{
    drv->optionalInh__INHERITS_type(T1, T2);
}

featureList(FL) ::= .
{
    drv->featureList(FL);
}

featureList(FL1) ::= featureList(FL2) feature(F) .
{
    drv->featureList__featureList_feature(FL1, FL2, F);
}

feature(F) ::= attr(A) .
{
    drv->feature__attr(F, A);
}

feature(F) ::= method(M) .
{
    drv->feature__method(F, M);
}

attr(A) ::= id(ID_) COLON type(T) optInit(E) SEMI  .
{
    drv->attr__id_COLON_type_optInit_SEMI(A, ID_, T, E);
}

method(M) ::= id(ID_) LPAREN formalsList(FL) RPAREN COLON type(T) LBRACE expr(E) RBRACE SEMI .
{
    drv->method__id_LPAREN_formalsList_RPAREN_COLON_type_LBRACE_expr_RBRACE_SEMI(M, ID_, FL, T, E);
}

formalsList(FL) ::= .
{
    drv->formalsList(FL);
}

formalsList(FL1) ::= formalsList(FL2) firstFormal(F) moreFormalsList(FL3) .
{
    drv->formalsList__formalsList_firstFormal_moreFormalsList(FL1, FL2, F, FL3);
}

firstFormal(F1) ::= formal(F2) .
{
    drv->firstFormal__formal(F1, F2);
}

moreFormalsList(FL1) ::= moreFormalsList(FL2) COMMA formal(F) .
{
    drv->moreFormalsList__moreFormalsList_COMMA_formal(FL1, FL2, F);
}

moreFormalsList(FL) ::= .
{
    drv->moreFormalsList(FL);
}

formal(F) ::= id(ID_) COLON type(T) .
{
    drv->formal__id_COLON_type(F, ID_, T);
}

id(ID_) ::= ID .
{
    drv->id(ID_);
}
type(T) ::= TYPE .
{
    drv->type(T);
}

expr(E) ::= FALSE .
{
    drv->expr__FALSE(E);
}

expr(E) ::= TRUE .
{
    drv->expr__TRUE(E);
}

optInit(E) ::= .
{
    drv->optInit(E);
}

optInit(E1) ::= LARROW expr(E2) .
{
    drv->optInit__LARROW_expr(E1, E2);
}