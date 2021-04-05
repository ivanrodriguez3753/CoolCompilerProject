#ifndef COOLCOMPILERPROJECT_ENVIRONMENT_H
#define COOLCOMPILERPROJECT_ENVIRONMENT_H

#include <string>
#include <map>
#include <iostream>
#include "llvm/IR/Value.h"

using namespace std;

class rec;
class classRec;
class methodRec;
class letCaseRec;
class objRec;
class env;
class globalEnv;
class classEnv;
class methodEnv;
class letCaseEnv;

class _symTable;
class _class;
class _method;
class _let;

/**
 * Base class for all record types
 */
class rec {
protected:
    const int lineno;
    rec(_symTable* node, int l) : treeNode(node), lineno(l) {}

public:
    _symTable* treeNode;
};

class classRec : public rec{
public:
    classRec(_class* node, int l, string p, int numA, int numM, classEnv* link) : rec((_symTable*)node, l), parent(p), numAttr(numA), numMethods(numM), link(link) {}
    const string parent;

    classEnv* link;

    const int numAttr;
    const int numMethods;

};
class methodRec : public rec {
public:
    methodRec(_method* node, int l, int nA, methodEnv* link, int e, string rt, string& d) : rec((_symTable*)node, l), numArgs(nA), link(link), encountered(e), returnType(rt), definer(d) { }
    methodEnv* link;

    string& definer;

    const int numArgs;
    int localOffset;
    const string returnType;
    int encountered;
};
class letCaseRec : public rec {
public:
    letCaseEnv* link;
    const int numLocals;

    letCaseRec(_let* node, int l, int nL, letCaseEnv* link) : rec((_symTable*)node, l), numLocals(nL), link(link) {}
};

class objRec : public rec {
public:
    objRec(_symTable* node, int l, int o, string t) : rec(node, l), localOffset(o), type(t) {}
    /**
     * Used for offsetting off sp[i >= firstAttrOffset] for attributes,
     * fp[i >= 0] for method arguments, or fp[i < 0] for local variables.
     */
    const int localOffset;
    const string type;
};



class env {
public:
    string id;

    map<string, rec*> symTable;
    env* prev;

    env(env* p, string i) : prev(p), id(i) {}
public:
    virtual rec* getRec(const string& key) const = 0;
};

class globalEnv : public env {
public:
    classRec* getRec(const string& key) const override {
        if(symTable.find(key) == symTable.end()) {
            return nullptr;
        }
        return (classRec*)symTable.at(key);
    }
    globalEnv(env* p, string i) : env(p, i) {}
};
class classEnv : public env {
public:
    classEnv(env* p, string i) : env(p, i) {}
    methodRec* getMethodRec(const string& key) const {
        return (methodRec*)methodsSymTable.at(key);
    }
    objRec* getRec(const string& key) const override {
        if(symTable.find(key) == symTable.end()) {
            return nullptr;
        }
        return (objRec*)symTable.at(key);
    }
    map<string, methodRec*> methodsSymTable;

};
class methodEnv : public env {
public:
    /**
     * This map keeps track of all the available local identifiers for a function, which is known at compile time
     * Name mangling will be
     *      (letCaseEnv.id) + id  OR
     *      "methodParam." + id
     * for the keys, and the value is a pointer to the result of alloca,
     * where alloca is called once for each local identifier at the beginning of the LLVM code for each user function.
     * Initialization for a methodParam is passed in as a single pointer, stored into the result of alloca for that parameter
     * Initialization for a let/case identifier (i.e. calling its ..ctr) happens only in the block in which that identifier
     * is declared in the cool source code. For example,
     * if isTrue then
     *      let x : Int ...
     * else
     *      let x : Int ...
     * fi
     * We would alloca for each x above, at the beginning of the LLVM user function definition, but we
     * call Int..ctr only in the block in which that identifier is introduced. Something like the following:
     * %let1.x = alloca %Int_c
     * %let2.x = alloca %Int_c
     *
     * %boolCheck = some boolean based on isTrue above
     * br i1 %boolCheck label %truePath, label %falsePath
     * truePath:
     *      call Int..ctr and pass in %let1.x
     *      ...do stuff
     *      br label %fi
     * falsePath:
     *      call Int..ctr and pass in %let2.x
     *      ...do stuff
     *      br label %fi
     * fi:
     *      ...whatever comes after the if statement
     */
    map<string, llvm::Value*> localsMap;



    methodEnv(env* p, string i) : env(p, i) {}
    objRec* getRec(const string& key) const override {
        if(symTable.find(key) == symTable.end()) {
            return nullptr;
        }
        return (objRec*)symTable.at(key);
    }
};
class letCaseEnv : public env {
public:
    letCaseEnv* prevLetCase;
    letCaseEnv(letCaseEnv* p, string i) : env((env*)p, i), prevLetCase(p) {}
    objRec* getRec(const string& key) const override {
        if(symTable.find(key) == symTable.end()) {
            return nullptr;
        }
        return (objRec*)symTable.at(key);
    }
    map<string, letCaseRec*> letCaseSymTable;
};


#endif //COOLCOMPILERPROJECT_ENVIRONMENT_H
