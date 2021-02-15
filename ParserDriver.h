//
// Created by Ivan Rodriguez on 2/13/21.
//

#ifndef COOLCOMPILERPROJECT_PARSERDRIVER_H
#define COOLCOMPILERPROJECT_PARSERDRIVER_H
#include "lexer.yy.hpp"
#include "parse.hpp"
#include <queue>
#include <stack>
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

class ParserDriver {
private:
    yyscan_t scanner;
public:
    ParserDriver() {}
    ~ParserDriver() {}

    void parse(const string& fileName) {
        yyscan_t scanner;
        yylex_init(&scanner);
        //TODO: open file
        string fileContents = "class Main1{attr1 : Int; attr2 : Bool; method1() : ReturnType{}; }; class Main2{}; class Main3{};";
        YY_BUFFER_STATE bufferState = yy_scan_string(fileContents.c_str(), scanner);

        //set up the parser
        void* parse = ParseAlloc(malloc);

        int lexCode;
        do {
            lexCode = yylex(scanner);
            Parse(parse, lexCode, NULL, this);
            if(lexCode == ID || lexCode == TYPE) {
                stringQ.push(yyget_text(scanner));
            }

        } while (lexCode > 0);

        //clean up the scanner and parser
        yy_delete_buffer(bufferState, scanner);
        yylex_destroy(scanner);
        ParseFree(parse, free);
    }


    _program* ast;
    int ctr = 0;



    /**
     * Q's for terminals that have information associated with them (bool is also used for differentiating between attr/method features
     *
     */
    queue<int> intQ;
    queue<bool> boolQ;
    stack<string> stringQ;


    void program__classList(vector<_class*>*& CL) {
        ast = new _program(*CL);
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

        C = new _class*;
        *C = new _class(*T, *SUPER_T, *FL);

    }
    void optionalInh(string*& T) {
        T = new string;
        stringQ.push("Object");
    }
    void featureList(pair<vector<_attr*>, vector<_method*>>*& FL) {
        FL = new pair<vector<_attr*>, vector<_method*>>;
    }
    void attr__id_COLON_type_SEMI(_attr**& A, string*& ID_, string*& T) {
        A = new _attr*;
        *T = stringQ.top(); stringQ.pop();
        *ID_ = stringQ.top(); stringQ.pop();
        *A = new _attr(*ID_, *T);
    }
    void feature__attr(featureUnion& F, _attr**& A) {
        F.attr = A;
        boolQ.push(true);
    }
    void feature__method(featureUnion& F, _method**& M) {
        F.method = M;
        boolQ.push(false);
    }

    void method__id_LPAREN_formalsList_RPAREN_COLON_type_LBRACE_RBRACE_SEMI(_method**& M, string*& ID_, vector<pair<string, string>>* FL, string*& T) {
        M = new _method*;
        *T = stringQ.top(); stringQ.pop();
        *ID_= stringQ.top(); stringQ.pop();
        *M = new _method(*ID_, *T, *FL);
    }
    void formalsList(vector<pair<string,string>>*&FL) {
        FL = new vector<pair<string,string>>;
    }

    void formalsList__formal(vector<pair<string,string>>*& FL, pair<string,string>*& F) {
        (*FL).push_back(*F);
    }
    void formalsList__formal_moreFormals(vector<pair<string, string>>*& FL, pair<string,string>*& F, vector<pair<string,string>>*& MF) {
        (*FL).push_back(*F);
        for(pair<string, string> formal : *MF) {
            (*FL).push_back(formal);
        }
    }
    void moreFormals__moreFormals_COMMA_formal(vector<pair<string, string>>*& MF1, vector<pair<string,string>>*& MF2, pair<string,string>*& F) {
        MF1 = MF2;
        (*MF1).push_back(*F);
    }
    void formal__id_COLON_type(pair<string,string>*& F, string*& ID_, string *& T) {
        F = new pair<string,string>;

    }
    void id(string*& ID_) {
        ID_ = new string;
        //*ID_ = cTocpp;
    }
    void type(string*& T) {
        T = new string;
        //*T = cTocpp;
    }



};



#endif //COOLCOMPILERPROJECT_PARSERDRIVER_H
