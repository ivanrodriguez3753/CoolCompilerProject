#ifndef COOLCOMPILERPROJECTALL_ENVIRONMENT_H
#define COOLCOMPILERPROJECTALL_ENVIRONMENT_H

#include <string>
#include <map>
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

    /**
     * to keep track of order of appearance. A map has a different internal ordering than we want (but maps
     * in general do not have an ordering property. The implementation does though)
     */
    static int orderCounter;

    Record(string lex, int l, string k);
};

class methodRecord : public Record {
public:
    string returnType;

    _method* treeNode;

    methodRecord(string lex, int l, string k, string rt);

public:
    static Environment* makeAndInstallMethodsRecordAndEnv(list<string>& lexemes, list<list<pair<string,string>>>& parameters, list<string>& returnTypes, Environment* current);
};

class classRecord : public Record {
public:
    string parent;
    _class* treeNode;

    classRecord(string lex, int l, string k, string p);
};

class objectRecord : public Record {
public:
    string type;
    _expr* initExpr;

    objectRecord(string lex, int l, string k, string type, _expr* init);
};

class Environment {
public:
    /**
     * methods and attributes can have the same identifier, so use a pair as key <identifier, kind>
     * For example, <"someName", "attribute"> is different from <"someName", "method">
     */
    map<pair<string, string>, Record*> symTable;

    Environment* previous;
    map<pair<string, string>, Environment*> links;

    Environment(Environment* prev);

    void install(pair<string, string>key, Record* rec);
    Record* get(pair<string, string> key);

    void reset();
};

extern Environment* globalEnv;
extern Environment* top;

#endif //COOLCOMPILERPROJECTALL_ENVIRONMENT_H
