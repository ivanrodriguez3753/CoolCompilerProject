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
    class featureListNode;
    class featureNode;
    class fieldNode;
    class exprNode;
    class optionalInitNode;
    class booleanNode;
    class methodNode;
    class formalNode;
    class formalsListNode;


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
%nterm <classListNode*> classList
%nterm <classNode*> class
%nterm <featureListNode*> featureList
%nterm <featureNode*> feature
%nterm <optionalInhNode*> optionalInh
%nterm <fieldNode*> field
%nterm <methodNode*> method
%nterm <exprNode*> expr
%nterm <optionalInitNode*> optionalInit
%nterm <formalsListNode*> formalsList
%nterm <formalNode*> formal
%nterm <formalNode*> firstFormal
%nterm <formalsListNode*> moreFormalsList


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
    classList
    {
        //this should be the only rule that doesn't use $$ =
        //instead it uses the global rootIVAN which is the root of the parse tree
        rootIVAN = new programNode{"program",
                                   $1};
    }
;

classList:
    classList class
    {
        $$ = $1;
        $1->classList.push_back($2);
        $1->children->push_back($2);

    }
|   %empty
	{
        $$ = new classListNode{"classList"};
	}

;


class:
    CLASS TYPE optionalInh LBRACE featureList RBRACE SEMI
    {


        $$ = new classNode{"classNode",
                           new terminalNode{"class"},
                           new wordNode{"type", $2},
                           $3,
                           new terminalNode{"lbrace"},
                           $5,
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
        $$ = new optionalInhNode{"optionalInhNode",
                                 new terminalNode{"inherits"},
                                 new wordNode{"type", $2}};
    }
;


featureList:
	%empty
	{
	    $$ = new featureListNode{"featureList"};
	}
|   featureList feature
    {
        $$ = $1;
        $1->featureList.push_back($2);
        $1->children->push_back($2);
    }
;

feature:
    field
    {
        $$ = $1;
    }
|   method
    {
        $$ = $1;
    }
;


field:
    IDENTIFIER COLON TYPE optionalInit SEMI
    {
        $$ = new fieldNode{"fieldNode",
                           new wordNode{"identifier", $1},
                           new terminalNode{"colon"},
                           new wordNode{"type", $3},
                           $4,
                           new terminalNode{"semi"}};
    }
;

optionalInit:
    %empty
    {
        $$ = nullptr;
    }
|   LARROW expr
    {
        $$ = new optionalInitNode{"optionalInit",
                                  new terminalNode{"larrow"},
                                  $2};
    }
;

expr:
    FALSE
    {
        $$ = new exprNode{"expr",
                          new booleanNode{"false", $1}};
    }
;

method:
    IDENTIFIER LPAREN formalsList RPAREN COLON TYPE LBRACE expr RBRACE SEMI
    {
        $$ = new methodNode{"method",
                            new wordNode{"identifier", $1},
                            new terminalNode{"lparen"},
                            $3,
                            new terminalNode{"rparen"},
                            new terminalNode{"colon"},
                            new wordNode{"type", $6},
                            new terminalNode{"lbrace"},
                            $8,
                            new terminalNode{"rbrace"},
                            new terminalNode{"semi"}};
    }
;


formalsList:
    formalsList firstFormal moreFormalsList
    {
        $$ = $1;
        $$->formalsList.push_back($2);
        $$->children->push_back($2);
        if($3 != nullptr) {
            for(auto child : *$3->children) {
                $$->children->push_back(child);
            }
        }
    }
|   %empty
    {
        $$ = new formalsListNode{"formalsList"};
    }
;

firstFormal:
    formal
    {
        $$ = $1;
    }

moreFormalsList:
    moreFormalsList COMMA formal
    {
        $$ = $1;
        $$->formalsList.push_back($3);
        $$->children->push_back(new terminalNode{"comma"});
        $$->children->push_back($3);
    }
|   %empty
    {
        $$ = new formalsListNode{"formalsList"};
    }
;


formal:
    IDENTIFIER COLON TYPE
    {
        $$ = new formalNode{"formal",
                            new wordNode{"identifier", $1},
                            new terminalNode{"colon"},
                            new wordNode{"type", $3}};
    }
;


%%
void yy::parser::error(const location_type& l, const std::string& m) {
    std::cerr << l << ": " << m << '\n';
}
