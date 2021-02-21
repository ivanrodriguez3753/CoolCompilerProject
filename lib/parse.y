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
%type argsList {vector<_expr*>*}
%type moreArgsList {vector<_expr*>*}
%type firstArg {_expr**}
%type id {string*}
%type type {string*}
%type str {string*}
%type optionalInh{string*}
%type optInit{_expr**}
%type argList{vector<_expr*>*}
%type exprList{vector<_expr*>*}
%type lbrace{int}

%type feature {featureUnion}
%type attr {_attr**}
%type method {_method**}
%type expr {_expr**}
%extra_argument {ParserDriver* drv}

%right LARROW .
%right NOT .
%nonassoc LE LT EQUALS .
%left PLUS MINUS .
%left TIMES DIVIDE .
%left ISVOID .
%left TILDE .
%left AT .
%left DOT .

program ::= classList(CL) .
{
    drv->program__classList(CL);
}

program ::= ELSE FALSE IF FI IN INHERITS ISVOID LET LOOP POOL THEN WHILE CASE ESAC NEW OF NOT TRUE COLON LPAREN RPAREN LARROW DOT COMMA AT PLUS MINUS TIMES DIVIDE NEG LT LE RARROW EQ INT ID STR.
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
str(S) ::= STR .
{
    drv->str(S);
}

expr(E) ::= FALSE .
{
    drv->expr__FALSE(E);
}

expr(E) ::= TRUE .
{
    drv->expr__TRUE(E);
}

expr(E) ::= INT .
{
    drv->expr__INT(E);
}

expr(E) ::= id(ID_) .
{
    drv->expr__id(E, ID_);
}

expr(E) ::= str(S) .
{
    drv->expr__str(E, S);
}

expr(E) ::= id(ID_) LPAREN argsList(AL) RPAREN .
{
    drv->expr__id_LPAREN_argsList__RPAREN(E, ID_, AL);
}

expr(E1) ::= expr(E2) DOT id(ID_) LPAREN argsList(AL) RPAREN .
{
    drv->expr__expr_DOT_id_LPAREN_argsList_RPAREN(E1, E2, ID_, AL);
}

expr(E1) ::= expr(E2) AT type(T) DOT id(ID_) LPAREN argsList(AL) RPAREN .
{
    drv->expr__expr_AT_type_DOT_id_LPAREN_argsList_RPAREN(E1, E2, T, ID_, AL);
}

expr(E1) ::= IF expr(E2) THEN expr(E3) ELSE expr(E4) FI .
{
    drv->expr__IF_expr_THEN_expr_ELSE_expr_FI(E1, E2, E3, E4);
}

expr(E1) ::= WHILE expr(E2) LOOP expr(E3) POOL .
{
    drv->expr__WHILE_expr_LOOP_expr_POOL(E1, E2, E3);
}

expr(E1) ::= id(ID_) LARROW expr(E2) .
{
    drv->expr__id_LARROW_expr(E1, ID_, E2);
}

expr(E1) ::= lbrace(l) exprList(EL) RBRACE .
{
    drv->expr__lbrace_exprList_RBRACE(E1, l, EL);
}

expr(E) ::= NEW type(T) .
{
    drv->expr__NEW_type(E, T);
}

expr(E1) ::= ISVOID expr(E2) .
{
    drv->expr__ISVOID_expr(E1, E2);
}

exprList(EL1) ::= exprList(EL2) expr(E) SEMI .
{
    drv->exprList__exprList_expr_SEMI(EL1, EL2, E);
}

exprList(EL) ::= .
{
    drv->exprList(EL);
}

optInit(E) ::= .
{
    drv->optInit(E);
}

optInit(E1) ::= LARROW expr(E2) .
{
    drv->optInit__LARROW_expr(E1, E2);
}

argsList(AL) ::= .
{
    drv->argsList(AL);
}

argsList(AL1) ::= argsList(AL2) firstArg(E) moreArgsList(AL3) .
{
    drv->argsList__argsList_firstArg_moreArgsList(AL1, AL2, E, AL3);
}

firstArg(E1) ::= expr(E2) .
{
    drv->firstArg__expr(E1, E2);
}

moreArgsList(AL1) ::= moreArgsList(AL2) COMMA expr(E) .
{
    drv->moreArgsList__moreArgsList_COMMA_expr(AL1, AL2, E);
}

moreArgsList(AL) ::= .
{
    drv->moreArgsList(AL);
}

lbrace(l) ::= LBRACE .
{
    drv->lbrace__LBRACE(l);
}