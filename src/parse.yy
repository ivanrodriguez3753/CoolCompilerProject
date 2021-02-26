%skeleton "lalr1.cc" //because we're making a C++ parser
%require "3.7.2"

%defines

//Scanner returns only genuine tokens and never simple characters
%define api.token.raw

%define api.token.constructor //benefit from type safety and more natural definition of "symbol"
%define api.value.type variant //semantic values are genuine C++ objects
%define parse.assert



%code requires {
    class node;

    #include <string>
    #include <list>
    #include <vector>



    class ParserDriver;

    class _node;
    class _symTable;
    class _env;
    class _program;
    class _class;
    class _feature;
    class _method;
    class _attr;
    class _let;
    class _letBinding;
    class _case;
    class _formal;
    class _expr;
    class _bool;
    class _int;
    class _id;
    class _dispatch;
    class _selfDispatch;
    class _dynamicDispatch;
    class _staticDispatch;
    class _if;
    class _while;
    class _assign;
    class _string;
    class _block;
    class _new;
    class _isvoid;
    class _arith;
    class _relational;
    class _unary;
    class _case;
    class _caseElement;

    class environment;
    class methodEnv;

    using namespace std;
}

//ParserDriver is passed by ref to parser and scanner. Provides simple but effective pure interface without globals

//parsing context
%param {ParserDriver& drv}

//request location tracking
%locations

%define parse.trace
%define parse.error detailed
%define parse.lac full


//This will be output in the *.cc file, it needs detailed knowledge about the driver
%code {
    #include "ParserDriver.hh"
    #include "ast.h"
    #include "environment.h"
    #include <iostream>

    environment* lastEnv;
}

//prefix tokens with TOK to avoid name clashes in generated files
%define api.token.prefix {TOK_}
%token
    AT          "at"
    CASE        "case"
    CLASS       "class"
    COLON       "colon"
    COMMA       "comma"
    DIVIDE      "divide"
    DOT         "dot"
    ELSE        "else"
    EQUALS      "equals"
    ESAC        "esac"
    FI          "fi"
    IF          "if"
    IN          "in"
    INHERITS    "inherits"
    ISVOID      "isvoid"
    LARROW      "larrow"
    LBRACE      "lbrace"
    LE          "le"
    LET         "let"
    LOOP        "loop"
    LPAREN      "lparen"
    LT          "lt"
    MINUS       "minus"
    NEW         "new"
    NOT         "not"
    OF          "of"
    PLUS        "plus"
    POOL        "pool"
    RARROW      "rarrow"
    RBRACE      "rbrace"
    RPAREN      "rparen"
    SEMI        "semi"
    THEN        "then"
    TILDE       "tilde"
    TIMES       "times"
    WHILE       "while"

//using variant based semantic values
//so declare the expected types
%token <std::string> IDENTIFIER "identifier"
%token <int> INTEGER "integer"
%token <std::string> STRING "string"
%token <bool> TRUE "true"
%token <bool> FALSE "false"
%token <std::string> TYPE "type"

%nterm <_program*> program
%nterm <vector<_class*>> classList
%nterm <_class*> class
%nterm <pair<vector<_attr*>, vector<_method*>>> featureList
%nterm <pair<_feature*, bool>> feature
%nterm <pair<string, int>> optionalInh
%nterm <_attr*> attr
%nterm <_method*> method
%nterm <_expr*> expr
%nterm <_expr*> optInit
%nterm <vector<_formal*>> formalsList
%nterm <_formal*> formal
%nterm <_formal*> firstFormal
%nterm <vector<_formal*>> moreFormalsList
%nterm <vector<_expr*>> exprList
%nterm <vector<_expr*>> moreExprList
%nterm <_expr*> firstExpr
%nterm <vector<_expr*>> blockExprList
%nterm <_letBinding*> firstBinding
%nterm <_letBinding*> binding
%nterm <vector<_letBinding*>> bindingList
%nterm <vector<_letBinding*>> moreBindingList
%nterm <vector<_caseElement*>> caseList
%nterm <_case*> case
%nterm <_caseElement*> caseElement



//don't need %destructor during error recovery, memory will be reclaimed by regular destructors
//all values are printed using their operator<<
//%printer {yyo << $$;} <*>;

//from the reference manual:

//The precedence of infix binary and prefix unary operations, from highest to lowest, is given by the following table:
//.
//@
//~
//isvoid
//* /
//+ -
//<=  <  =
//not
//<-
//All binary operations are left-associative, with the exception of assignment, which is right-associative, and the three comparison operations, which do not associate.


//bison needs precedence to be from lowest to highest
%right LARROW
%right NOT
%nonassoc LE LT EQUALS
%left PLUS MINUS
%left TIMES DIVIDE
%left ISVOID
%left TILDE
%left AT
%left DOT

//start grammar
%%

%start program;
program:
    classList
    {
        $$ = new _program(0, $1);
        drv.bisonProduct = $$;
    }
;

classList:
    classList class
    {
        $$ = $1;
        $$.push_back($2);
    }
|   %empty
;

class:
    CLASS TYPE optionalInh LBRACE featureList RBRACE SEMI
    {
        $$ = new _class(@2.begin.line, $3.second, $2, $3.first, $5);
    }
;

optionalInh:
    %empty
    {
        $$ = make_pair("Object", 0);
    }
|   INHERITS TYPE
    {
        $$ = make_pair($2, @2.begin.line);
    }
;

featureList:
	featureList feature
    {
        $$ = $1;
        if($2.second) $$.first.push_back((_attr*)($2.first)); //attribute
        else $$.second.push_back((_method*)($2.first)); //method
    }
|   %empty
;

feature:
    attr
    {
        $$ = make_pair($1, true);
    }
|   method
    {
        $$ = make_pair($1, false);
    }
;


attr:
    IDENTIFIER COLON TYPE optInit SEMI
    {
        $$ = new _attr(@1.begin.line, @3.begin.line, $1, $3, $4, drv.encountered++);
    }
;

optInit:
    LARROW expr
    {
        $$ = $2;
    }
|   %empty
    {
        $$ = nullptr;
    }
;

expr:
    IDENTIFIER LARROW expr
    {
        $$ = new _assign(@1.begin.line, $1, $3);
    }
|   IDENTIFIER LPAREN exprList RPAREN
    {
        $$ = new _selfDispatch(@1.begin.line, $1, $3);
    }
|   expr DOT IDENTIFIER LPAREN exprList RPAREN
    {
        $$ = new _dynamicDispatch(@3.begin.line, $3, $5, $1);
    }
|   expr AT TYPE DOT IDENTIFIER LPAREN exprList RPAREN
    {
        $$ = new _staticDispatch(@1.begin.line, $5, $7, $1, $3, @3.begin.line);
    }
|   IF expr THEN expr ELSE expr FI
    {
        $$ = new _if(@1.begin.line, $2, $4, $6);
    }
|   WHILE expr LOOP expr POOL
    {
        $$ = new _while(@1.begin.line, $2, $4);
    }
|   LBRACE blockExprList RBRACE
    {
        $$ = new _block(@1.begin.line, $2);
    }
|   LET bindingList IN expr
    {
        $$ = new _let(@1.begin.line, "let" + to_string(drv.encountered), $2, $4, drv.encountered++);
    }
|   CASE expr OF caseList ESAC
    {
        $$ = new _case(@1.begin.line, "case" + to_string(drv.encountered), $4, $2, drv.encountered++);
    }
|   NEW TYPE
    {
        $$ = new _new(@1.begin.line, $2, @2.begin.line);
    }
|   ISVOID expr
    {
        $$ = new _isvoid(@1.begin.line, $2);
    }
|   expr PLUS expr
    {
        $$ = new _arith(@1.begin.line, $1, _arith::OPS::PLUS, $3);
    }
|   expr MINUS expr
    {
        $$ = new _arith(@1.begin.line, $1, _arith::OPS::MINUS, $3);
    }
|   expr TIMES expr
    {
        $$ = new _arith(@1.begin.line, $1, _arith::OPS::TIMES, $3);
    }
|   expr DIVIDE expr
    {
        $$ = new _arith(@1.begin.line, $1, _arith::OPS::DIVIDE, $3);
    }
|   TILDE expr
    {
        $$ = new _unary(@1.begin.line, $2, _unary::OPS::NEG);
    }
|   NOT expr
    {
        $$ = new _unary(@1.begin.line, $2, _unary::OPS::NOT);
    }
|   expr LT expr
    {
        $$ = new _relational(@1.begin.line, $1, _relational::OPS::LT, $3);
    }
|   expr LE expr
    {
        $$ = new _relational(@1.begin.line, $1, _relational::OPS::LE, $3);

    }
|   expr EQUALS expr
    {
        $$ = new _relational(@1.begin.line, $1, _relational::OPS::EQUALS, $3);
    }
|   LPAREN expr RPAREN
    {
        $$ = $2;
    }
|   IDENTIFIER
    {
        $$ = new _id(@1.begin.line, $1);
    }
|   INTEGER
    {
        $$ = new _int(@1.begin.line, $1);
    }
|   STRING
    {
        $$ = new _string(@1.begin.line, $1);
    }
|   TRUE
    {
        $$ = new _bool(@1.begin.line, true);
    }
|   FALSE
    {
        $$ = new _bool(@1.begin.line, false);
    }
;

caseList:
    caseList caseElement
    {
        $$ = $1;
        $$.push_back($2);
    }
|   %empty
;

caseElement:
    IDENTIFIER COLON TYPE RARROW expr SEMI
    {
        $$ = new _caseElement(@1.begin.line, $1, @3.begin.line, $3, $5);
    }
;

blockExprList:
    blockExprList expr SEMI
    {
        $$ = $1;
        $$.push_back($2);
    }
|   %empty
;

method:
    IDENTIFIER LPAREN formalsList RPAREN COLON TYPE LBRACE expr RBRACE SEMI
    {
        $$ = new _method(@1.begin.line, @6.begin.line, $1, $6, $3, $8, drv.encountered++);
    }
;

bindingList:
    bindingList firstBinding moreBindingList
    {
        $$ = $1;
        $$.push_back($2);
        for(_letBinding* letBinding : $3) {
            $$.push_back(letBinding);
        }
    }
|   %empty
;

firstBinding:
    binding
    {
        $$ = $1;
    }
;

moreBindingList:
    moreBindingList COMMA binding
    {
        $$ = $1;
        $$.push_back($3);
    }
|   %empty
;

binding:
    IDENTIFIER COLON TYPE optInit
    {
        $$ = new _letBinding(@1.begin.line, $1, @3.begin.line, $3, $4);
    }
;

exprList:
    exprList firstExpr moreExprList
    {
        $$ = $1;
        $$.push_back($2);
        for(_expr* expr : $3) {
            $$.push_back(expr);
        }
    }
|   %empty
;

firstExpr:
    expr
    {
        $$ = $1;
    }
;

moreExprList:
    moreExprList COMMA expr
    {
        $$ = $1;
        $$.push_back($3);
    }
|   %empty
;

formalsList:
    formalsList firstFormal moreFormalsList
    {
        $$ = $1;
        $$.push_back($2);
        for(_formal* formal : $3) {
            $$.push_back(formal);
        }
    }
|   %empty
;

firstFormal:
    formal
    {
        $$ = $1;
    }
;
moreFormalsList:
    moreFormalsList COMMA formal
    {
        $$ = $1;
        $$.push_back($3);
    }
|   %empty
;
formal:
    IDENTIFIER COLON TYPE
    {
        $$ = new _formal(@1.begin.line, @3.begin.line, $1, $3);
    }
;


%%
void yy::parser::error(const location_type& l, const std::string& m) {
    std::cerr << l << ": " << m << '\n';
}