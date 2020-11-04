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
    class boolExprNode;
    class exprListNode;
    class ifExprNode;
    class whileExprNode;
    class blockExprNode;
    class letExprNode;
    class bindingListNode;
    class bindingNode;
    class caseExprNode;
    class caseListNode;
    class caseNode;
    class arithExprNode;
    class relExprNode;
    class unaryExprNode;
    class termExprNode;
    class identifierExprNode;
    class intExprNode;
    class stringExprNode;
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
%nterm <exprListNode*> exprList
%nterm <exprListNode*> moreExprList
%nterm <exprNode*> firstExpr
%nterm <exprListNode*> blockExprList
%nterm <bindingNode*> firstBinding
%nterm <bindingNode*> binding
%nterm <bindingListNode*> bindingList
%nterm <bindingListNode*> moreBindingList
%nterm <caseListNode*> caseList
%nterm <caseNode*> case



//don't need %destructor during error recovery, memory will be reclaimed by regular destructors
//all values are printed using their operator<<
%printer {yyo << $$;} <*>;

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
        //this should be the only rule that doesn't use $$ =
        //instead it uses the global rootIVAN which is the root of the parse tree
        rootIVAN = new programNode{"program",
                                   "program",
                                   $1};
        rootIVAN->lineNo = @$.begin.line;
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
        $$->lineNo = @$.begin.line;
	}

;


class:
    CLASS TYPE optionalInh LBRACE featureList RBRACE SEMI
    {
        string productionBody = "no_inherits";
        if($3 != nullptr) {
            productionBody = "inherits";
        }

        $$ = new classNode{"classNode",
                           productionBody,
                           new terminalNode{"class"},
                           new wordNode{"type", $2},
                           $3,
                           new terminalNode{"lbrace"},
                           $5,
                           new terminalNode{"rbrace"},
                           new terminalNode{"semi"}
                           };
       $$->lineNo = @$.begin.line;
       $$->TYPE->lineNo = @2.begin.line;
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
        $$->lineNo = @$.begin.line;
        $$->TYPE->lineNo = @2.begin.line;

    }
;


featureList:
	%empty
	{
	    $$ = new featureListNode{"featureList"};
        $$->lineNo = @$.begin.line;
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
        string productionBody = "attribute_no_init";
        if($4 != nullptr) {
            productionBody = "attribute_init";
        }
        $$ = new fieldNode{"fieldNode",
                           productionBody,
                           new wordNode{"identifier", $1},
                           new terminalNode{"colon"},
                           new wordNode{"type", $3},
                           $4,
                           new terminalNode{"semi"}};
        $$->lineNo = @$.begin.line;
        $$->IDENTIFIER->lineNo = @1.begin.line;
        $$->TYPE->lineNo = @3.begin.line;
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
        $$->lineNo = @$.begin.line;
    }
;

expr:
    IDENTIFIER LARROW expr
    {
        $$ = new assignExprNode{"expr",
                                "assign",
                                new wordNode{"identifier", $1},
                                new terminalNode{"larrow"},
                                $3};
        $$->lineNo = @$.begin.line;
        ((assignExprNode*)$$)->IDENTIFIER->lineNo = @1.begin.line;
    }
|   IDENTIFIER LPAREN exprList RPAREN
    {
        $$ = new selfDispatchNode{"expr",
                                  "self_dispatch",
                                  new wordNode{"identifier", $1},
                                  new terminalNode{"lparen"},
                                  $3,
                                  new terminalNode{"rparen"}};
        $$->lineNo = @$.begin.line;
        ((selfDispatchNode*)$$)->IDENTIFIER->lineNo = @1.begin.line;
    }
|   expr DOT IDENTIFIER LPAREN exprList RPAREN
    {
        $$ = new dynamicDispatchNode{"expr",
                                     "dynamic_dispatch",
                                     $1,
                                     new terminalNode{"dot"},
                                     new wordNode{"identifier", $3},
                                     new terminalNode{"lparen"},
                                     $5,
                                     new terminalNode{"rparen"}};
        $$->lineNo = @$.begin.line;
        ((dynamicDispatchNode*)$$)->IDENTIFIER->lineNo = @3.begin.line;
    }
|   expr AT TYPE DOT IDENTIFIER LPAREN exprList RPAREN
    {
        $$ = new staticDispatchNode{"expr",
                                    "static_dispatch",
                                    $1,
                                    new terminalNode{"at"},
                                    new wordNode{"type", $3},
                                    new terminalNode{"dot"},
                                    new wordNode{"identifier", $5},
                                    new terminalNode{"lparen"},
                                    $7,
                                    new terminalNode{"rparen"}};
        $$->lineNo = @$.begin.line;
        ((staticDispatchNode*)$$)->TYPE->lineNo = @3.begin.line;
        ((staticDispatchNode*)$$)->IDENTIFIER->lineNo = @5.begin.line;

    }
|   IF expr THEN expr ELSE expr FI
    {
        $$ = new ifExprNode{"expr",
                            "if",
                            new terminalNode{"if"},
                            $2,
                            new terminalNode{"then"},
                            $4,
                            new terminalNode{"else"},
                            $6,
                            new terminalNode{"fi"}
                            };
        $$->lineNo = @$.begin.line;
    }
|   WHILE expr LOOP expr POOL
    {
        $$ = new whileExprNode{"expr",
                               "while",
                               new terminalNode{"while"},
                               $2,
                               new terminalNode{"loop"},
                               $4,
                               new terminalNode{"pool"}
                              };
        $$->lineNo = @$.begin.line;
    }
|   LBRACE blockExprList RBRACE
    {
        $$ = new blockExprNode{"expr",
                               "block",
                               new terminalNode{"lbrace"},
                               $2,
                               new terminalNode{"rbrace"}
                              };
        $$->lineNo = @$.begin.line;
    }
|   LET bindingList IN expr
    {
        $$ = new letExprNode{"expr",
                             "let",
                             new terminalNode{"let"},
                             $2,
                             new terminalNode{"in"},
                             $4
                            };
        $$->lineNo = @$.begin.line;
    }
|   CASE expr OF caseList ESAC
    {
        $$ = new caseExprNode{"expr",
                              "case",
                              new terminalNode{"case"},
                              $2,
                              new terminalNode{"of"},
                              $4,
                              new terminalNode{"esac"}
                             };
        $$->lineNo = @$.begin.line;
    }
|   NEW TYPE
    {
        $$ = new newExprNode{"expr",
                             "new",
                             new terminalNode{"new"},
                             new wordNode{"type", $2}
                            };
        $$->lineNo = @$.begin.line;
        ((newExprNode*)$$)->TYPE->lineNo = @2.begin.line;
    }
|   ISVOID expr
    {
        $$ = new isvoidExprNode{"expr",
                                "isvoid",
                                new terminalNode{"isvoid"},
                                $2
                               };
        $$->lineNo = @$.begin.line;
    }
|   expr PLUS expr
    {
        $$ = new arithExprNode{"expr",
                               "plus",
                              $1,
                              new terminalNode{"plus"},
                              $3
                             };
        $$->lineNo = @$.begin.line;
    }
|   expr MINUS expr
    {
        $$ = new arithExprNode{"expr",
                               "minus",
                              $1,
                              new terminalNode{"minus"},
                              $3
                             };
        $$->lineNo = @$.begin.line;
    }
|   expr TIMES expr
    {
        $$ = new arithExprNode{"expr",
                               "times",
                              $1,
                              new terminalNode{"times"},
                              $3
                             };
        $$->lineNo = @$.begin.line;
    }
|   expr DIVIDE expr
    {
        $$ = new arithExprNode{"expr",
                               "divide",
                              $1,
                              new terminalNode{"divide"},
                              $3
                             };
        $$->lineNo = @$.begin.line;
    }
|   TILDE expr
    {
        $$ = new unaryExprNode{"expr",
                               "negate",
                               new terminalNode{"tilde"},
                               $2
                              };
        $$->lineNo = @$.begin.line;
    }
|   NOT expr
    {
        $$ = new unaryExprNode{"expr",
                               "not",
                               new terminalNode{"not"},
                               $2
                              };
        $$->lineNo = @$.begin.line;
    }
|   expr LT expr
    {
        $$ = new relExprNode{"expr",
                             "lt",
                             $1,
                             new terminalNode{"lt"},
                             $3
                            };
        $$->lineNo = @$.begin.line;
    }
|   expr LE expr
    {
        $$ = new relExprNode{"expr",
                             "le",
                             $1,
                             new terminalNode{"le"},
                             $3
                            };
        $$->lineNo = @$.begin.line;
    }
|   expr EQUALS expr
    {
        $$ = new relExprNode{"expr",
                             "eq",
                             $1,
                             new terminalNode{"equals"},
                             $3
                            };
        $$->lineNo = @$.begin.line;
    }
|   LPAREN expr RPAREN
    {
        $$ = new termExprNode{"expr",
                              "termExpr", //this is not in the list of possible syntax nodes, it'll just pass through and become an expression
                              new terminalNode{"lparen"},
                              $2,
                              new terminalNode{"rparen"}
                             };
        $$->lineNo = @$.begin.line;
    }
|   IDENTIFIER
    {
        $$ = new identifierExprNode{"expr",
                                    "identifier",
                                    new wordNode{"identifier", $1}
                                   };
        $$->lineNo = @$.begin.line;
        ((identifierExprNode*)$$)->IDENTIFIER->lineNo = @1.begin.line;
    }
|   INTEGER
    {
        $$ = new intExprNode{"expr",
                             "integer",
                             new integerNode{"integer", $1}
                            };
        $$->lineNo = @$.begin.line;
    }
|   STRING
    {
        $$ = new stringExprNode{"expr",
                                "string",
                                new wordNode{"string", $1}
                               };
        $$->lineNo = @$.begin.line;
    }
|   TRUE
    {
        $$ = new boolExprNode{"expr",
                              "true",
                              new booleanNode{"true", $1}
                             };
        $$->lineNo = @$.begin.line;

    }
|   FALSE
    {
        $$ = new boolExprNode{"expr",
                              "false",
                              new booleanNode{"false", $1}};
        $$->lineNo = @$.begin.line;
    }
;

caseList:
    caseList case
    {
        $$ = $1;
        $$->caseList.push_back($2);
        $$->children->push_back($2);
    }
|   %empty
    {
        $$ = new caseListNode{"expr"};
        $$->lineNo = @$.begin.line;
    }
;

case:
    IDENTIFIER COLON TYPE RARROW expr SEMI
    {
        $$ = new caseNode{"expr",
                          "caseElement",
                          new wordNode{"identifier", $1},
                          new terminalNode{"colon"},
                          new wordNode{"type", $3},
                          new terminalNode{"rarrow"},
                          $5,
                          new terminalNode{"semi"}
                          };
        $$->lineNo = @$.begin.line;
        ((caseNode*)$$)->IDENTIFIER->lineNo = @1.begin.line;
        ((caseNode*)$$)->TYPE->lineNo = @3.begin.line;
    }
;

blockExprList:
    %empty
    {
        $$ = new exprListNode{"exprList"};
        $$->lineNo = @$.begin.line;
    }
|   blockExprList expr SEMI
    {
        $$ = $1;
        $$->exprList.push_back($2);
        $$->children->push_back($2);
        $$->children->push_back(new terminalNode{"semi"});
    }
;



method:
    IDENTIFIER LPAREN formalsList RPAREN COLON TYPE LBRACE expr RBRACE SEMI
    {
        $$ = new methodNode{"method",
                            "method",
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
        $$->IDENTIFIER->lineNo = @1.begin.line;
        $$->TYPE->lineNo = @6.begin.line;
    }
;

bindingList:
    bindingList firstBinding moreBindingList
    {
        $$ = $1;
        $$->bindingList.push_back($2);
        $$->children->push_back($2);
        if($3 != nullptr) {
            for(auto child : *$3->children) { //push back bindings and commas
                $$->children->push_back(child);
            }
            for(auto binding : $3->bindingList) {//push only bindings
                $$->bindingList.push_back(binding);
            }
        }
    }
|   %empty
    {
        $$ = new bindingListNode{"bindingList"};
        $$->lineNo = @$.begin.line;
    }
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
        $$->bindingList.push_back($3);
        $$->children->push_back(new terminalNode{"comma"});
        $$->children->push_back($3);
    }
|   %empty
    {
        $$ = new bindingListNode{"bindingList"};
        $$->lineNo = @$.begin.line;
    }
;

binding:
    IDENTIFIER COLON TYPE optionalInit
    {
        string productionBody = "let_binding_no_init";
        if($4 != nullptr) {
            productionBody = "let_binding_init";
        }
        $$ = new bindingNode{"expr",
                             productionBody,
                             new wordNode{"identifier", $1},
                             new terminalNode{"colon"},
                             new wordNode{"type", $3},
                             $4
                            };
        $$->lineNo = @$.begin.line;
        ((bindingNode*)$$)->IDENTIFIER->lineNo = @1.begin.line;
        ((bindingNode*)$$)->TYPE->lineNo = @3.begin.line;
    }
;

exprList:
    exprList firstExpr moreExprList
    {
        $$ = $1;
        $$->exprList.push_back($2);
        $$->children->push_back($2);
        if($3 != nullptr) {
            for(auto child : *$3->children) { //push back COMMAs and exprs
                $$->children->push_back(child);
            }
            for(auto expr : $3->exprList) {//push only exprs
                $$->exprList.push_back(expr);
            }
        }
    }
|   %empty
    {
        $$ = new exprListNode{"exprList"};
        $$->lineNo = @$.begin.line;
    }
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
        $$->exprList.push_back($3);
        $$->children->push_back(new terminalNode{"comma"});
        $$->children->push_back($3);
    }
|   %empty
    {
        $$ = new exprListNode{"exprList"};
        $$->lineNo = @$.begin.line;
    }
;


formalsList:
    formalsList firstFormal moreFormalsList
    {
        $$ = $1;
        $$->formalsList.push_back($2);
        $$->children->push_back($2);
        if($3 != nullptr) {
            for(auto child : *$3->children) { //push COMMAs and formals
                $$->children->push_back(child);
            }
            for(auto formal : $3->formalsList) {//push only formals
                $$->formalsList.push_back(formal);
            }
        }
    }
|   %empty
    {
        $$ = new formalsListNode{"formalsList"};
        $$->lineNo = @$.begin.line;
    }
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
        $$->formalsList.push_back($3);
        $$->children->push_back(new terminalNode{"comma"});
        $$->children->push_back($3);
    }
|   %empty
    {
        $$ = new formalsListNode{"formalsList"};
        $$->lineNo = @$.begin.line;
    }
;


formal:
    IDENTIFIER COLON TYPE
    {
        $$ = new formalNode{"formal",
                            "formal",
                            new wordNode{"identifier", $1},
                            new terminalNode{"colon"},
                            new wordNode{"type", $3}};
        $$->IDENTIFIER->lineNo = @1.begin.line;
        $$->TYPE->lineNo = @3.begin.line;
    }
;


%%
void yy::parser::error(const location_type& l, const std::string& m) {
    std::cerr << l << ": " << m << '\n';
}
