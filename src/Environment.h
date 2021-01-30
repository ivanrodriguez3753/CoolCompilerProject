#ifndef COOLCOMPILERPROJECTALL_ENVIRONMENT_H
#define COOLCOMPILERPROJECTALL_ENVIRONMENT_H

#include <string>
#include <map>
#include <algorithm>
#include "syntaxTreeNodes.h"
#include "Environment.h"

class _expr;
class _class;
class _method;

class Environment;

using namespace std;

class Record {

public:
    string lexeme;
    int lineNo;
    string kind;
    int encountered;

    //A reference to the containing environment
    const Environment* containerEnv;

    /**
     * to keep track of order of appearance. A map has a different internal ordering than we want (but maps
     * in general do not have an ordering property. The implementation does though)
     */
    static int orderCounter;

    Record(Environment* container, string lex, int l, string k);
};

class methodRecord : public Record {
public:
    int maxIdentifiers;
    string returnType;

    _method* treeNode;

    methodRecord(Environment* container, string lex, int l, string k, string rt);

public:
    static void makeAndInstallMethodsRecordAndEnv(list<string> lexemes, vector<vector<pair<string,string>>> parameters, list<string> returnTypes, Environment* current);
};

class classRecord : public Record {
public:
    string parent;
    _class* treeNode;

    int numAttributes;

    classRecord(string lex, int l, string k, string p);
};

class objectRecord : public Record {
public:
    /**
     * Refactor this out later but for local objects, keep track of frame pointer offset
     */
    int fpOffset;

    string type;
    _expr* initExpr;

    objectRecord(Environment* container, string lex, int l, string k, string type, _expr* init);
};

class Environment {
public:
    struct envMetaInfo {
        string identifier, kind;
        int depth; //global env is depth 0

        envMetaInfo(string id, string k) : identifier{id}, kind{k} {
        }
    }metaInfo;

    /**
     * methods and attributes can have the same identifier, so use a pair as key <identifier, kind>
     * For example, <"someName", "attribute"> is different from <"someName", "method">
     */
    map<pair<string, string>, Record*> symTable;
    map<pair<string, string>, Environment*> links;
    Environment* previous;

    //nothing exists outside a class so it is convenient to have a reference back to the containing class environment
    Environment* klass;

    Environment(Environment* prev, envMetaInfo info);

    void install(pair<string, string>key, Record* rec);
    Record* get(pair<string, string> key);
    objectRecord* getObject(string key);

    void reset();

    //convenience method for type checking
    string O(string key);
        /**Environment consists of three parts: method environment M, object environment O, and current class C
         * mapping M(C, f) = (T1,...,Tn, T_n+1) where C is a type, f is a methodName, T1,...,Tn are types, and T_n+1 is the return type
         * mapping O(v) = T where v is an object identifier, T is a type
         * current class C in which the expression appears
         */
    static vector<string> M(string C, string f);

    string C;


    /**
     * returns a vector containing a vector of "methods" in the symTable
     * @return
     */
    vector<methodRecord*> getMethods();
};

extern Environment* globalEnv;
extern Environment* top;

#endif //COOLCOMPILERPROJECTALL_ENVIRONMENT_H
