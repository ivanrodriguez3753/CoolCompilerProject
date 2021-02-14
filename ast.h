#ifndef COOLCOMPILERPROJECT_AST_H
#define COOLCOMPILERPROJECT_AST_H

#include "environment.h"
#include <list>
#include <vector>

class node;
class _symTable;
class _program;
class _class;


class _node {

};

/**
 * Nodes that have a corresponding record must have minimum information, so that the ast
 * encompasses only the structure that we cannot encapsulate in the symbol table.
 */
class _symTable : public _node {
protected:
    env* selfEnv;
public:
    virtual env* getEnv() const = 0;
};

class  _program : public _symTable {
private:
public:
    vector<_class*> classList;

//    _program() {}

    _program(vector<_class*> cL) : classList(cL) {

    }




    globalEnv* getEnv() const override {
        return (globalEnv*)selfEnv;
    }

};

class _class : public _symTable {
public:
    const string id;
    const string superId;

    _class(string id, string sId = "Object") : id(id), superId(sId) {

    }


    classEnv* getEnv() const override {
        return (classEnv*)selfEnv;
    }
};

class _method : public _symTable {
public:
    methodEnv* getEnv() const override {
        return (methodEnv*)selfEnv;
    }
};

class _let : public _symTable {
    letCaseEnv* getEnv() const override {
        return (letCaseEnv*)selfEnv;
    }
};

class _case : public _symTable {

};


#endif //COOLCOMPILERPROJECT_AST_H
