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

    void parse(const string& fileName) {
        yylex_init(&scanner);
        //TODO: open file
        string fileContents =
            "class\n"
            "Main1{\n"
            "   attr1\n"
            "       : Int;\n"
            "   attr2\n"
            "       : Bool;\n"
            "   method1\n"
            "      (arg1\n"
            "            : Int, \n"
            "       arg2\n"
            "           : String)\n"
            "       : ReturnType{\n"
            "   };\n"
            "}; \n"
            "class Main2{\n"
            "}; \n"
            "class Main3{\n"
            "};\n";
        YY_BUFFER_STATE bufferState = yy_scan_string(fileContents.c_str(), scanner);
        yyset_lineno(1, scanner);


        //set up the parser
        void* parse = ParseAlloc(malloc);

        int lexCode;
        do {
            lexCode = yylex(scanner);
            Parse(parse, lexCode, NULL, this);
            if(lexCode == ID || lexCode == TYPE) {
                stringQ.push(yyget_text(scanner));
                lineNoStack.push(yyget_lineno(scanner));
            }
        } while (lexCode > 0);

        cout << "BEGIN printing leftover lineNoStack!\n";
        while(!lineNoStack.empty()) {
            cout << lineNoStack.top() << endl;
            lineNoStack.pop();
        }
        cout << "END printing leftover lineNoStack!\n";

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
    queue<int> intQ;
    queue<bool> boolQ;
    stack<string> stringQ;
    stack<int> lineNoStack;


    void program__classList(vector<_class*>*& CL) {
        ast = new _program(0, *CL);
    }
    void classList__classList_class(vector<_class*>*& CL1, vector<_class*>*& CL2, _class**& C) {
        CL1 = CL2;
        (*CL1).push_back(*C);
    }
    void featureList__featureList_feature(pair<vector<_attr*>, vector<_method*>>*& FL1, pair<vector<_attr*>,vector<_method*>>*& FL2, featureUnion& F) {
        FL1 = FL2;
        if(boolQ.front()) { //attribute
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
        *SUPER_T = stringQ.top(); stringQ.pop();
        *T = stringQ.top(); stringQ.pop();
        int tl = lineNoStack.top(); lineNoStack.pop();
        int l = lineNoStack.top(); lineNoStack.pop();


        C = new _class*;
        *C = new _class(l, tl, *T, *SUPER_T, *FL);

    }
    void optionalInh(string*& T) {
        T = new string;
        stringQ.push("Object");
        lineNoStack.push(0);

    }
    void featureList(pair<vector<_attr*>, vector<_method*>>*& FL) {
        FL = new pair<vector<_attr*>, vector<_method*>>;
    }
    void attr__id_COLON_type_SEMI(_attr**& A, string*& ID_, string*& T) {
        A = new _attr*;
        *T = stringQ.top(); stringQ.pop();
        *ID_ = stringQ.top(); stringQ.pop();
        int tl = lineNoStack.top(); lineNoStack.pop();
        int l = lineNoStack.top(); lineNoStack.pop();
        *A = new _attr(l, tl, *ID_, *T);
    }
    void feature__attr(featureUnion& F, _attr**& A) {
        F.attr = A;
        boolQ.push(true);
    }
    void feature__method(featureUnion& F, _method**& M) {
        F.method = M;
        boolQ.push(false);
    }

    void method__id_LPAREN_formalsList_RPAREN_COLON_type_LBRACE_RBRACE_SEMI(_method**& M, string*& ID_, vector<_formal*>* FL, string*& T) {
        M = new _method*;
        *T = stringQ.top(); stringQ.pop();
        *ID_= stringQ.top(); stringQ.pop();
        int tl = lineNoStack.top(); lineNoStack.pop();
        int l = lineNoStack.top(); lineNoStack.pop();
        *M = new _method(l, tl, *ID_, *T, *FL);
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
        *T = stringQ.top(); stringQ.pop();
        *ID_= stringQ.top(); stringQ.pop();
        int tl = lineNoStack.top(); lineNoStack.pop();
        int l = lineNoStack.top(); lineNoStack.pop();
        *F = new _formal(l, tl, "", "");
    }
    void id(string*& ID_) {
        ID_ = new string;
    }
    void type(string*& T) {
        T = new string;
    }



};



#endif //COOLCOMPILERPROJECT_PARSERDRIVER_H
