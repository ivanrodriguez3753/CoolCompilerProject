#ifndef COOLCOMPILERPROJECT_AST_H
#define COOLCOMPILERPROJECT_AST_H

#include "environment.h"
#include <string>
#include <list>
#include <vector>
#include <iostream>

class node;
class _symTable;
class _program;
class _class;
class _method;
class _attr;
class _formal;
class _expr;
class letCaseEnv;

using namespace std;

class _node {
public:

    virtual void print(ostream& os) const = 0;
    virtual void prettyPrint(ostream& os, string prefix) const = 0;

    _node(int l) : lineNo(l) {}
protected:
    const static string T;
    const static string indent;

    const int lineNo;

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

    _symTable(int l, string i) : _node(l), id(i) {}

    virtual rec* getSelfRec() const = 0;
};

class _env : public _symTable {
protected:
    env* selfEnv;

    virtual env* getSelfEnv() const = 0;

    _env(int l, string id) : _symTable(l, id) {}
};

class  _program : public _env {
private:
public:
    vector<_class*> classList;


    _program(int l, vector<_class*> cL) : _env(l, "global"), classList(cL) {
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

    void prettyPrint(ostream&os, string indentPrefix) const override;
    void print(ostream& os) const override;
};

class _class : public _env {
public:
    const string superId;
    const int superLineNo;

    pair<vector<_attr*>, vector<_method*>> featureList;

    classRec* rec;

    _class(int l, int sl, string id, string sId, pair<vector<_attr*>, vector<_method*>> fL) : _env(l, id), superId(sId), featureList(fL), superLineNo(sl) {
    }


    classEnv* getSelfEnv() const override {
        return (classEnv*)selfEnv;
    }
    classRec* getSelfRec() const override {
        return (classRec*) selfRec;
    }

    void prettyPrint(ostream& os, const string indentPrefix) const;
    void print(ostream& os) const override;

};

class _feature {
public:
    int encountered;
    bool isAttr = false;
};

class _method : public _env, public _feature {
public:

    _method(int l, int tl, string i, string rT, vector<_formal*> fL, _expr* b) : _env(l, i), returnType(rT), formalsList(fL), typeLineNo(tl), body(b) {
    }

    /**
     * <id, type>
     */
    vector<_formal*> formalsList;
    string returnType;
    const int typeLineNo;
    _expr* body;

    methodEnv *getSelfEnv() const override {
        return (methodEnv *) selfEnv;
    }

    methodRec *getSelfRec() const override {
        return (methodRec *) selfRec;
    }

    void prettyPrint(ostream &os, const string indentPrefix) const;
    void print(ostream& os) const override;

};

class _attr : public _symTable, public _feature {
public:
    const string type;
    const int typeLineNo;

    _expr* optInit;

    _attr(int l, int tl, string i, string t, _expr* oI) : _symTable(l, i), type(t), typeLineNo(tl), optInit(oI) {
    }

    objRec* getSelfRec() const override {
        return (objRec*)selfRec;
    }
    void prettyPrint(ostream &os, const string indentPrefix) const override;
    void print(ostream& os) const override;


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
public:
    string type;
    const int typeLineNo;

    _formal(int l, int tl, string i, string t) : _symTable(l, i), type(t), typeLineNo(tl) {

    }

    letCaseRec* getSelfRec() const override {
        return (letCaseRec*)selfRec;
    }

    void prettyPrint(ostream &os, const string indentPrefix) const override;
    void print(ostream& os) const override;
};

class _expr : public _node {
public:
    _expr(int l) : _node(l) {}
};

class _bool : public _expr {
public:
    const bool value;

    void prettyPrint(ostream &os, const string indentPrefix) const override;
    void print(ostream& os) const override;

    _bool(int l, bool v) : _expr(l), value(v) {}
};

class _int : public _expr {
public:
    const int value;

    _int(int l, int v) : _expr(l), value(v) {}

    void prettyPrint(ostream &os, const string indentPrefix) const override {}
    void print(ostream& os) const override;
};

class _id : public _expr {
public:
    const string value;

    void prettyPrint(ostream &os, const string indentPrefix) const override {}
    void print(ostream& os) const override;


    _id(int l, string v) : _expr(l), value(v) { }
};

class _dispatch : public _expr {
public:
    const string id;
    vector<_expr*> argList;

    _dispatch(int l, string i, vector<_expr*> aL) : _expr(l), id(i), argList(aL) {}
};

class _selfDispatch : public _dispatch {
public:
    _selfDispatch(int l, string i, vector<_expr*> aL) : _dispatch(l, i, aL) {}

    void print(ostream& os) const override;
    void prettyPrint(ostream &os, const string indentPrefix) const override {}
};

class _dynamicDispatch : public _dispatch {
public:
    _expr* caller;

    _dynamicDispatch(int l, string i, vector<_expr*> aL, _expr* c) : _dispatch(l, i, aL), caller(c) {}

    void print(ostream& os) const override;
    void prettyPrint(ostream &os, const string indentPrefix) const override {}
};

class _staticDispatch : public _dispatch {
public:
    _expr* caller;
    const string staticType;
    int typeLineNo;

    _staticDispatch(int l, string i, vector<_expr*> aL, _expr* c, string s, int tL) : _dispatch(l, i, aL), caller(c), staticType(s), typeLineNo(tL) {}

    void print(ostream& os) const override;
    void prettyPrint(ostream &os, const string indentPrefix) const override {}
};

class _if : public _expr {
public:
    _expr* predicate;
    _expr* tthen;
    _expr* eelse;

    _if(int l, _expr* p, _expr* t, _expr* e) : _expr(l), predicate(p), tthen(t), eelse(e) {}

    void print(ostream& os) const override;
    void prettyPrint(ostream &os, const string indentPrefix) const override {}
};

class _while : public _expr {
public:
    _expr* predicate;
    _expr* body;

    _while(int l, _expr* p, _expr* b) : _expr(l), predicate(p), body(b) {}

    void print(ostream& os) const override;
    void prettyPrint(ostream &os, const string indentPrefix) const override {}
};

class _assign : public _expr {
public:
    const string id;
    _expr* rhs;

    _assign(int l, string i, _expr* r) : _expr(l), id(i), rhs(r) {}

    void print(ostream& os) const override;
    void prettyPrint(ostream &os, const string indentPrefix) const override {}
};

class _string : public _expr {
public:
    const string value;

    void prettyPrint(ostream &os, const string indentPrefix) const override {}
    void print(ostream& os) const override;

    _string(int l, string v) : _expr(l), value(v) {}

};

class _block : public _expr {
public:
    vector<_expr*> body;

    void prettyPrint(ostream &os, const string indentPrefix) const override {}
    void print(ostream& os) const override;

    _block(int l, vector<_expr*> b) : _expr(l), body(b) {}
};

#endif //COOLCOMPILERPROJECT_AST_H
