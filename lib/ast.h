#ifndef COOLCOMPILERPROJECT_AST_H
#define COOLCOMPILERPROJECT_AST_H

#include "environment.h"
#include <list>
#include <vector>
#include <iostream>

class node;
class _symTable;
class _program;
class _class;
class _method;
class _attr;
class letCaseEnv;

using namespace std;

class _node {

//    virtual void print(ostream& os) const = 0;
public:
    void print(ostream& os) const;
    void prettyPrint(ostream& os, string prefix) const;

protected:
    const static string T;
    const static string indent;


//    const int encountered;
//    const int lineNo;
};

/**
 * Nodes that have a corresponding record must have minimum information, so that the ast
 * encompasses only the structure that we cannot encapsulate in the symbol table.
 */
class _symTable : public _node {
protected:
    rec* selfRec;
public:
    string id;

    _symTable(string i) : id(i) {}

    virtual rec* getSelfRec() const = 0;
};

class _env : public _symTable {
protected:
    env* selfEnv;

    virtual env* getSelfEnv() const = 0;

    _env(string id) : _symTable(id) {}
};

class  _program : public _env {
private:
public:
    vector<_class*> classList;

//    _program() {}

    _program(vector<_class*> cL) : _env("global"), classList(cL) {
    }




    globalEnv* getSelfEnv() const override {
        return (globalEnv*)selfEnv;
    }

    /**
     * This is the global, root node per parser instance, and globals do not have an associated record.
     * TODO: later implement a programRec type since we're gonna put all the parallel bits together in a global-global tree
     * @return nullptr
     */
    rec* getSelfRec() const override {
        return nullptr;
    }

    void prettyPrint(ostream&os, const string prefix) const;

};

class _class : public _env {
public:
    const string superId;

    pair<vector<_attr*>, vector<_method*>> featureList;

    classRec* rec;

    _class(string id, string sId, pair<vector<_attr*>, vector<_method*>> fL) : _env(id), superId(sId), featureList(fL) {
    }


    classEnv* getSelfEnv() const override {
        return (classEnv*)selfEnv;
    }
    classRec* getSelfRec() const override {
        return (classRec*) selfRec;
    }

    void prettyPrint(ostream& os, const string indentPrefix) const;
};

class _method : public _env {
public:

    _method(string i, string rT, vector<pair<string, string>> fL) : _env(i), returnType(rT), formalsList(fL) {
    }

    /**
     * <id, type>
     */
    vector<pair<string, string>> formalsList;
    string returnType;


    methodEnv *getSelfEnv() const override {
        return (methodEnv *) selfEnv;
    }

    methodRec *getSelfRec() const override {
        return (methodRec *) selfRec;
    }

    void prettyPrint(ostream &os, const string indentPrefix) const;
};

class _attr : public _symTable {
public:
    string type;

    _attr(string i, string t) : _symTable(i), type(t) {
    }

    objRec* getSelfRec() const override {
        return (objRec*)selfRec;
    }
    void prettyPrint(ostream &os, const string indentPrefix) const;

};

class _let : public _env {
    letCaseEnv* getSelfEnv() const override {
        return (letCaseEnv*)selfEnv;
    }
    letCaseRec* getSelfRec() const override {
        return (letCaseRec*)selfRec;
    }
};

class _case : public _env {
    letCaseEnv* getSelfEnv() const override {
        return (letCaseEnv*)selfEnv;
    }
    letCaseRec* getSelfRec() const override {
        return (letCaseRec*)selfRec;
    }
};

class _formal : public _symTable {
private:
    string type;
public:
    _formal(string i, string t) : _symTable(i), type(t) {

    }

};


#endif //COOLCOMPILERPROJECT_AST_H
