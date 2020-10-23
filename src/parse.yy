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

    class ParserDriver;
    class programNode;
    class classNode;
    class classListNode;
    class optionalInhNode;


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
    #include "parseTreeNodes.h"

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

//%nterm < std::list<classNode *>* > classlist
%nterm <programNode*> program
%nterm <classListNode*> classlist
%nterm <classNode*> class
%nterm <featureListNode*> featureList
%nterm <featureNode*> feature
%nterm <optionalInhNode*> optionalInh


//don't need %destructor during error recovery, memory will be reclaimed by regular destructors
//all values are printed using their operator<<
%printer {yyo << $$;} <*>;


//start grammar
%%
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
//%right "<-"
//%precedence NOT
//%precedence LE LT EQUALS
//%left "+" "-"
//%left "*" "/"
//%precedence ISVOID
//%precedence TILDE
//%left "@"
//%left "."

%start program;
program:
    classlist
    {
        //this should be the only rule that doesn't use $$ =
        //instead it uses the global rootIVAN which is the root of the parse tree
        rootIVAN = new programNode{$1};
    }
;

classlist:
    classlist class
    {
        $$ = $1;
        $1->classList.push_back($2);
        $1->children->push_back($2);

    }
|   %empty
	{
        $$ = new classListNode{};
	}

;


class: //took off optionalInh and featureList for now
    CLASS TYPE optionalInh LBRACE RBRACE SEMI
    {


        $$ = new classNode{new terminalNode{"class"},
                           new wordNode{"type", $2},
                           $3,
                           new terminalNode{"lbrace"},
                           new terminalNode{"rbrace"},
                           new terminalNode{"semi"}
                           };
    }
;

optionalInh:
    %empty
    {
        $$ = nullptr;
    }
|   INHERITS TYPE
    {
        $$ = new optionalInhNode{new terminalNode{"inherits"},
                                 new wordNode{"type", $2}};
    }
;


featurelist:
	/*epsilon*/
|   featureList feature
;

%%
void yy::parser::error(const location_type& l, const std::string& m) {
    std::cerr << l << ": " << m << '\n';
}
