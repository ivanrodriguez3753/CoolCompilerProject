//
// Created by Ivan Rodriguez on 2/13/21.
//

#ifndef COOLCOMPILERPROJECT_PARSERDRIVER_H
#define COOLCOMPILERPROJECT_PARSERDRIVER_H
#include "lexer.yy.hpp"
#include "parse.hpp"
#include "ast.h"
#include <queue>
#include <stack>
#include <vector>
#include <fstream>
#include <sstream>
/**
 * This class interfaces with flex and lemon. It carries thread-safe global data, as well as methods implementing
 * procedures associated with a production and methods managing the AST.
 */
class ParserDriver;

union featureUnion{
    _attr** attr;
    _method** method;
};

void* ParseAlloc(void* (*allocProc)(size_t));
void Parse(void* parser, int token, const char* tokenInfo, ParserDriver* info);
void ParseFree(void* parser, void(*freeProc)(void*));

using namespace std;

class ParserDriver {
private:
    yyscan_t scanner;
public:
    ParserDriver() {}
    ~ParserDriver() {}

    void parse(const string fileName) {
        yylex_init(&scanner);
        ifstream ifs(fileName);
        if(!ifs.good()) {
            cerr << "couldn't open file to parse!\n";
            abort();
        }
        //TODO maybe this is wasteful but need a string
        stringstream ss;
        ss << ifs.rdbuf();
        string fileContents = ss.str();
        ifs.close();

        YY_BUFFER_STATE bufferState = yy_scan_string(fileContents.c_str(), scanner);
        yyset_lineno(1, scanner);

        //set up the parser
        void* parse = ParseAlloc(malloc);

        int lexCode;
        do {
            lexCode = yylex(scanner);
            Parse(parse, lexCode, NULL, this);
            if(lexCode == ID || lexCode == TYPE) {
                stringStack.push(yyget_text(scanner));
                lineNoStack.push(yyget_lineno(scanner));
            }
            else if(lexCode == FALSE || lexCode == TRUE || lexCode == IF || lexCode == WHILE || lexCode == ISVOID || lexCode == NEW) {
                lineNoStack.push(yyget_lineno(scanner));
            }
            else if(lexCode == INT) {
                intStack.push(atoi(yyget_text(scanner)));
                lineNoStack.push(yyget_lineno(scanner));
            }
            else if(lexCode == STR) {
                stringStack.push(yyget_extra(scanner));
                lineNoStack.push(yyget_lineno(scanner));
            }
            else if(lexCode == LBRACE) {
                lastLBRACE_lineno = yyget_lineno(scanner);
            }
        } while (lexCode > 0);

        //clean up the scanner and parser
        yy_delete_buffer(bufferState, scanner);
        yylex_destroy(scanner);
        ParseFree(parse, free);
    }


    _program* ast;

    /**
     * used to properly order features, because my implementation does not have a node type _feature such that both of
     * _method, _attr <= _feature. Only needed for testing purposes
     */
    int ctr = 0;



    int encountered = 0;

    /**
     * Q's for terminals that have information associated with them (bool is also used for differentiating between attr/method features
     *
     */
    stack<int> intStack;
    stack<bool> boolQ;
    stack<string> stringStack;
    stack<int> lineNoStack;

    int lastLBRACE_lineno;

    void program__classList(vector<_class*>*& CL) {
        ast = new _program(0, *CL);
    }
    void classList__classList_class(vector<_class*>*& CL1, vector<_class*>*& CL2, _class**& C) {
        CL1 = CL2;
        (*CL1).push_back(*C);
    }
    void featureList__featureList_feature(pair<vector<_attr*>, vector<_method*>>*& FL1, pair<vector<_attr*>,vector<_method*>>*& FL2, featureUnion& F) {
        FL1 = FL2;
        if(boolQ.top()) { //attribute
            (*FL1).first.push_back(*(F.attr));
        } else { //method
            (*FL1).second.push_back(*(F.method));
        }
        boolQ.pop();
    }
    void classList(vector<_class*>*&  CL) {
        CL = new vector<_class*>;
    }
    void optionalInh__INHERITS_type(string*& T1, string*& T2) {
        T1 = T2;
    }
    void class__CLASS_type_optionalInh_LBRACE_featureList_RBRACE_SEMI(_class**& C, string*& T, string*& SUPER_T, pair<vector<_attr*>, vector<_method*> >*& FL){
        *SUPER_T = stringStack.top(); stringStack.pop();
        *T = stringStack.top(); stringStack.pop();
        int tl = lineNoStack.top(); lineNoStack.pop();
        int l = lineNoStack.top(); lineNoStack.pop();


        C = new _class*;
        *C = new _class(l, tl, *T, *SUPER_T, *FL);

    }
    void optionalInh(string*& T) {
        T = new string;
        stringStack.push("Object");
        lineNoStack.push(0);

    }
    void featureList(pair<vector<_attr*>, vector<_method*>>*& FL) {
        FL = new pair<vector<_attr*>, vector<_method*>>;
    }
    void attr__id_COLON_type_optInit_SEMI(_attr**& A, string*& ID_, string*& T, _expr**& E) {
        A = new _attr*;
        *T = stringStack.top(); stringStack.pop();
        *ID_ = stringStack.top(); stringStack.pop();
        int tl = lineNoStack.top(); lineNoStack.pop();
        int l = lineNoStack.top(); lineNoStack.pop();
        *A = new _attr(l, tl, *ID_, *T, *E);

        (*A)->encountered = encountered++;
        (*A)->isAttr = true;
    }
    void feature__attr(featureUnion& F, _attr**& A) {
        F.attr = A;
        boolQ.push(true);
    }
    void feature__method(featureUnion& F, _method**& M) {
        F.method = M;
        boolQ.push(false);
    }

    void method__id_LPAREN_formalsList_RPAREN_COLON_type_LBRACE_expr_RBRACE_SEMI(_method**& M, string*& ID_, vector<_formal*>*& FL, string*& T, _expr**& E) {
        M = new _method*;
        *T = stringStack.top(); stringStack.pop();
        *ID_= stringStack.top(); stringStack.pop();
        int tl = lineNoStack.top(); lineNoStack.pop();
        int l = lineNoStack.top(); lineNoStack.pop();
        *M = new _method(l, tl, *ID_, *T, *FL, *E);

        (*M)->encountered = encountered++;
    }

    void formalsList(vector<_formal*>*& FL) {
        FL = new vector<_formal*>;
    }
    void formalsList__formalsList_firstFormal_moreFormalsList(vector<_formal*>*& FL1, vector<_formal*>*& FL2, _formal**& F, vector<_formal*>*& FL3) {
        FL1 = FL2;
        (*FL1).push_back(*F);
        for(_formal* formal : *FL3) {
            (*FL1).push_back(formal);
        }
    }
    void firstFormal__formal(_formal**& F1, _formal**& F2) {
        F1 = F2;
    }
    void moreFormalsList__moreFormalsList_COMMA_formal(vector<_formal*>*& FL1, vector<_formal*>*& FL2, _formal**& F){
        FL1 = FL2;
        (*FL1).push_back(*F);
    }
    void moreFormalsList(vector<_formal*>*& FL) {
        FL = new vector<_formal*>;
    }
    void formal__id_COLON_type(_formal**& F, string*& ID_, string*& T) {
        F = new _formal*;
        *T = stringStack.top(); stringStack.pop();
        *ID_= stringStack.top(); stringStack.pop();
        int tl = lineNoStack.top(); lineNoStack.pop();
        int l = lineNoStack.top(); lineNoStack.pop();
        *F = new _formal(l, tl, *ID_, *T);
    }
    void id(string*& ID_) {
        ID_ = new string;
    }
    void type(string*& T) {
        T = new string;
    }
    void str(string*& S) {
        S = new string;
    }

    void expr__FALSE(_expr**& E) {
        E = new _expr*;
        int l = lineNoStack.top(); lineNoStack.pop();

        *E = new _bool(l, false);
    }

    void expr__TRUE(_expr**& E) {
        E = new _expr*;
        int l = lineNoStack.top(); lineNoStack.pop();

        *E = new _bool(l, true);
    }

    void optInit(_expr**& E) {
        E = new _expr*;
        *E = nullptr;
    }

    void optInit__LARROW_expr(_expr**& E1, _expr**& E2) {
        E1 = new _expr*;
        *E1 = *E2;
    }

    void expr__INT(_expr**& E) {
        E = new _expr*;
        int l = lineNoStack.top(); lineNoStack.pop();
        int v = intStack.top(); intStack.pop();

        *E = new _int(l, v);
    }

    void expr__id(_expr**& E, string*& ID_) {
        E = new _expr*;
        int l = lineNoStack.top(); lineNoStack.pop();
        *ID_ = stringStack.top(); stringStack.pop();

        *E = new _id(l, *ID_);
    }
    void expr__str(_expr**& E, string*& S) {
        E = new _expr*;
        int l = lineNoStack.top(); lineNoStack.pop();
        *S = stringStack.top(); stringStack.pop();

        *E = new _string(l, *S);
    }

    void expr__id_LPAREN_argsList__RPAREN(_expr**& E, string*& ID_, vector<_expr*>*& AL) {
        E = new _expr*;
        *ID_= stringStack.top(); stringStack.pop();
        int l = lineNoStack.top(); lineNoStack.pop();


        *E = new _selfDispatch(l, *ID_, *AL);
    }

    void expr__expr_DOT_id_LPAREN_argsList_RPAREN(_expr**& E1, _expr**& E2, string*& ID_, vector<_expr*>*& AL) {
            E1 = new _expr*;
            *ID_= stringStack.top(); stringStack.pop();
            int l = lineNoStack.top(); lineNoStack.pop();

            *E1 = new _dynamicDispatch(l, *ID_, *AL, *E2);
        }

    void expr__expr_AT_type_DOT_id_LPAREN_argsList_RPAREN(_expr**& E1, _expr**& E2, string*& T, string*& ID_, vector<_expr*>*& AL) {
        E1 = new _expr*;
        *ID_= stringStack.top(); stringStack.pop();
        *T = stringStack.top(); stringStack.pop();
        int l = lineNoStack.top(); lineNoStack.pop();
        int tL = lineNoStack.top(); lineNoStack.pop();

        *E1 = new _staticDispatch(l, *ID_, *AL, *E2, *T, tL);
    }

    void expr__IF_expr_THEN_expr_ELSE_expr_FI(_expr**& E1, _expr**& E2, _expr**& E3, _expr**& E4) {
        E1 = new _expr*;
        int l = lineNoStack.top(); lineNoStack.pop();

        *E1 = new _if(l, *E2, *E3, *E4);
    }

    void expr__WHILE_expr_LOOP_expr_POOL(_expr**& E1, _expr**& E2, _expr**& E3) {
        E1 = new _expr*;
        int l = lineNoStack.top(); lineNoStack.pop();

        *E1 = new _while(l, *E2, *E3);
    }

    void expr__id_LARROW_expr(_expr**& E1, string*& ID_, _expr**& E2) {
        E1 = new _expr*;
        int l = lineNoStack.top(); lineNoStack.pop();
        *ID_ = stringStack.top(); stringStack.pop();

        *E1 = new _assign(l, *ID_, *E2);
    }

    void expr__lbrace_exprList_RBRACE(_expr**& E1, int& l, vector<_expr*>*& EL) {
        E1 = new _expr*;

        *E1 = new _block(l, *EL);
    }

    void expr__NEW_type(_expr**& E, string*& T) {
        E = new _expr*;
        int tL = lineNoStack.top(); lineNoStack.pop();
        int l = lineNoStack.top(); lineNoStack.pop();
        *T = stringStack.top(); stringStack.pop();

        *E = new _new(l, *T, tL);
    }

    void expr__ISVOID_expr(_expr**& E1, _expr**& E2) {
        E1 = new _expr*;
        int l = lineNoStack.top(); lineNoStack.pop();

        *E1 = new _isvoid(l, *E2);
    }

    void  expr__expr_PLUS_expr(_expr**& E1, _expr**& E2, _expr**& E3) {
        E1 = new _expr*;

        *E1 = new _arith((*E2)->lineNo, *E2, '+', *E3);
    }

    void  expr__expr_MINUS_expr(_expr**& E1, _expr**& E2, _expr**& E3) {
        E1 = new _expr*;

        *E1 = new _arith((*E2)->lineNo, *E2, '-', *E3);
    }

    void  expr__expr_TIMES_expr(_expr**& E1, _expr**& E2, _expr**& E3) {
        E1 = new _expr*;

        *E1 = new _arith((*E2)->lineNo, *E2, '*', *E3);
    }

    void  expr__expr_DIVIDE_expr(_expr**& E1, _expr**& E2, _expr**& E3) {
        E1 = new _expr*;

        *E1 = new _arith((*E2)->lineNo, *E2, '/', *E3);
    }

    void exprList__exprList_expr_SEMI(vector<_expr*>*& EL1, vector<_expr*>*& EL2, _expr**& E) {
        EL1 = EL2;
        (*EL1).push_back(*E);
    }

    void exprList(vector<_expr*>*& EL) {
        EL = new vector<_expr*>;
    }

    void argsList(vector<_expr*>*& AL) {
        AL = new vector<_expr*>;
    }

    void argsList__argsList_firstArg_moreArgsList(vector<_expr*>*& AL1, vector<_expr*>*& AL2, _expr**& E, vector<_expr*>*& AL3) {
        AL1 = AL2;
        (*AL1).push_back(*E);
        for(_expr* expr : *AL3) {
            (*AL1).push_back(expr);
        }
    }

    void firstArg__expr(_expr**& E1, _expr**& E2) {
        E1 = E2;
    }

    void moreArgsList__moreArgsList_COMMA_expr(vector<_expr*>*& AL1, vector<_expr*>*& AL2, _expr**& E) {
        AL1 = AL2;
        (*AL1).push_back(*E);
    }

    void moreArgsList(vector<_expr*>*& AL) {
        AL = new vector<_expr*>;
    }

    void lbrace__LBRACE(int& l) {
        l = lastLBRACE_lineno;
    }

};



#endif //COOLCOMPILERPROJECT_PARSERDRIVER_H
