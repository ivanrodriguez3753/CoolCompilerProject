#ifndef COOLCOMPILERPROJECT_AST_H
#define COOLCOMPILERPROJECT_AST_H

#include "environment.h"

class _node {

};

class _symTable : public _node {
protected:
    env* selfEnv;
public:
    virtual env* getEnv() const = 0;
};

class  _program : public _symTable {
public:
    globalEnv* getEnv() const override {
        return (globalEnv*)selfEnv;
    }
};

class _class : public _symTable {
    const string superId;

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
