#ifndef COOLCOMPILERPROJECTALL_SYNTAXTREENODES_H
#define COOLCOMPILERPROJECTALL_SYNTAXTREENODES_H

#include <list>
#include <string>
#include "parseTreeNodes.h"
#include "Environment.h"

using namespace std;

class _node;
class _program;
class _class;
class _classNoInh;
class _classInh;
class _feature;
class _idMeta;
class _attributeInit;
class _attributeNoInit;
class _expr;
class _formal;
class classRecord;
class methodRecord;

//don't want basic classes on the AST, but I do want _class nodes for them
extern _class* Object_class;
extern _class* IO_class;
extern _class* Bool_class;
extern _class* String_class;
extern _class* Int_class;

extern map<string, _class*> basicClassNodes;

class _node {
public:
    int lineNo;
    _node(int l);

    virtual void print(ostream& os) const = 0;
    virtual void prettyPrint(ostream& os, string prefix) const;

    static bool isAnnotated;

    virtual void traverse() = 0;

    friend ostream& operator<<(ostream& os, const _node& n);
};

/**
 * this really shouldn't inherit from _node
 * defining its traverse method as useless
 */
class _idMeta : public _node{
public:
    string identifier;
    string kind;



    _idMeta(int l, string id, string k = "") : _node{l}, identifier{id}, kind{k} {}

    void traverse() {}

    void print(ostream &os) const override;
};

/**
 * A Cool program is merely a list of classes
 */
class _program : public _node {
public:
    list<_class*> classList;
    _program(int l) : _node{l} {}

    void traverse() override;

    void print(ostream &os) const override;
    void prettyPrint(ostream& os, string prefix) const;
};

class _class : public _node {
public:
    _idMeta typeIdentifier;
    list<_feature*> featureList;

    classRecord* rec;

    void traverse() override;

    _class(_idMeta id, classRecord* r);
};

class _classNoInh : public _class {
public:

    _classNoInh(_idMeta id, classRecord* r);

    void print(ostream& os) const override;
    void prettyPrint(ostream& os, string prefix) const;

};

class _classInh : public _class {
public:
    _idMeta superClassIdentifier;

    _classInh(_idMeta id, classRecord* r, _idMeta sId);
    void print(ostream& os) const override;
    void prettyPrint(ostream& os, string prefix) const;

};

class _feature : public _node {
public:
    _idMeta identifier;
    _idMeta typeIdentifier;

    virtual void traverse() = 0;

    _feature(int l, _idMeta id, _idMeta typeId);

};

class _attributeNoInit : public _feature {
public:
    _attributeNoInit(int l, _idMeta id, _idMeta typeId);


    //friend ostream& operator<<(ostream& os, const _attributeNoInit& a);
    void print(ostream& os) const override;
    void prettyPrint(ostream& os, string prefix) const;

    void traverse() override{} //dont need to do anything
};



class _attributeInit : public _feature {
public:
    _expr* expr;

    _attributeInit(int l, _idMeta id, _idMeta typeId, _expr* e);

    void print(ostream& os) const override;
    void prettyPrint(ostream& os, string prefix) const;

    void traverse() override;
};

class _method : public _feature {
public:
    list<_formal*> formalList;
    _expr* body;

    methodRecord* rec;

    _method(_idMeta id, _idMeta typeId, _expr* e);

    void print(ostream& os) const override;
    void prettyPrint(ostream& os, string prefix) const;

    void traverse() override;
};

class _formal : public _node {
public:
    _idMeta identifier;
    _idMeta typeIdentifier;

    void print(ostream &os) const override;
    void prettyPrint(ostream& os, string prefix) const;

    void traverse() override{}

    _formal(_idMeta id, _idMeta typeId);
};

class _expr : public _node {
public:
    _expr(int l);

    string exprType;

    virtual void traverse() = 0;

    static bool printExprType;
};

class _assign : public _expr {
public:
    _idMeta identifier;
    _expr* rhs;

    _assign(int l, _idMeta id, _expr* r);

    void traverse() override;

    void print(ostream& os) const override;
    void prettyPrint(ostream& os, string prefix) const;
};

class _dispatch : public _expr {
public:
    _idMeta method;
    list<_expr*> args;

    _dispatch(int l, _idMeta m);

    virtual void traverse() = 0;
};

class _dynamicDispatch : public _dispatch {
public:
    _expr* caller;

    _dynamicDispatch(int l, _idMeta m, _expr* e);

    void print(ostream& os) const override;
    void prettyPrint(ostream& os, string prefix) const;

    void traverse() override;
};

class _staticDispatch : public _dispatch {
public:
    _expr* caller;
    _idMeta typeIdentifier;

    _staticDispatch(int l, _idMeta m, _expr* e, _idMeta ty);

    void print(ostream& os) const override;
    void prettyPrint(ostream& os, string prefix) const;

    void traverse() override;
};

class _selfDispatch : public _dispatch {
public:
    _selfDispatch(int l, _idMeta m);

    void print(ostream& os) const override;
    void prettyPrint(ostream& os, string prefix) const;

    void traverse() override;
};

class _if : public _expr {
public:
    _expr* predicate;
    _expr* thenExpr;
    _expr* elseExpr;

    _if(int l, _expr* p, _expr* te, _expr* ee);

    void print(ostream& os) const override;
    void prettyPrint(ostream& os, string prefix) const;

    void traverse() override;
};

class _while : public _expr {
public:
    _expr* predicate;
    _expr* body;

    _while(int l, _expr* p, _expr* b);

    void print(ostream& os) const override;
    void prettyPrint(ostream& os, string prefix) const;

    void traverse() override;
};

class _block : public _expr {
public:
    list<_expr*> body;

    _block(int l);

    void print(ostream& os) const override;
    void prettyPrint(ostream& os, string prefix) const;

    void traverse() override;
};

class _new : public _expr {
public:
    _idMeta identifier;

    _new(int l, _idMeta id);

    void print(ostream& os) const override;
    void prettyPrint(ostream& os, string prefix) const;

    void traverse() override;
};

class _isvoid : public _expr {
public:
    _expr* expr;

    _isvoid(int l, _expr* e);

    void print(ostream& os) const override;
    void prettyPrint(ostream& os, string prefix) const;

    void traverse() override;
};

class _arith : public _expr {
public:
    _expr* left;
    string op; //+ - * / but in words
    _expr* right;

    _arith(int l, _expr* le, string o, _expr* r);

    void print(ostream& os) const override;
    void prettyPrint(ostream& os, string prefix) const;

    void traverse() override;
};

class _relational : public _expr {
public:
    _expr* left;
    string op; // < <= - but with letters: lt le eq
    _expr* right;

    _relational(int l, _expr* le, string o, _expr* r);

    void print(ostream& os) const override;
    void prettyPrint(ostream& os, string prefix) const;

    void traverse() override;
};

class _unary : public _expr {
public:
    string op; //not or ~, using words not or negate
    _expr* expr;

    _unary(int l, string o, _expr* e);

    void print(ostream& os) const override;
    void prettyPrint(ostream& os, string prefix) const;

    void traverse() override;
};

class _integer : public _expr {
public:
    int value;

    _integer(int l, int v);

    void print(ostream& os) const override;
    void prettyPrint(ostream& os, string prefix) const;

    void traverse() override{}
};

class _string : public _expr {
public:
    string value;

    _string(int l, string v);

    void print(ostream& os) const override;
    void prettyPrint(ostream& os, string prefix) const;

    void traverse() override{} //don't need to do anything
};

class _identifier : public _expr {
public:
    _idMeta identifier;

    _identifier(int l, _idMeta id);

    void print(ostream& os) const override;
    void prettyPrint(ostream& os, string prefix) const;

    void traverse() override;
};

class _bool : public _expr {
public:
    bool value;

    _bool(int l, bool v);

    void print(ostream& os) const override;
    void prettyPrint(ostream& os, string prefix) const;

    void traverse() override{}
};

class _letBinding : public _expr {
public:
    _idMeta identifier;
    _idMeta typeIdentifier;

    _expr* body;

    _letBinding(_idMeta id, _idMeta typeId);

    virtual void traverse() = 0;
};

class _letBindingNoInit : public _letBinding {
public:
    _letBindingNoInit(_idMeta id, _idMeta typeId);

    void print(ostream& os) const override;
    void prettyPrint(ostream& os, string prefix) const;

    void traverse() override{} //Don't need to do anything
};

class _letBindingInit : public _letBinding {
public:
    _expr* init;

    _letBindingInit(_idMeta id, _idMeta typeId, _expr* i);

    void print(ostream& os) const override;
    void prettyPrint(ostream& os, string prefix) const;

    void traverse() override;
};

class _let : public _expr {
public:
    list<_letBinding*> bindings;
    _expr* body;

    _idMeta letKey;
    static int letCounter;

    _let(int l, _idMeta lk, _expr* b);

    void print(ostream& os) const override;
    void prettyPrint(ostream& os, string prefix) const;

    void traverse() override;
};

class _caseElement : public _expr {
public:
    _idMeta identifier;
    _idMeta typeIdentifier;
    _expr* body;

    _idMeta caseKey;
    static int caseCounter;

    _caseElement(_idMeta id, _idMeta typeId, _expr* b, _idMeta ck);

    void print(ostream& os) const override;
    void prettyPrint(ostream& os, string prefix) const;

    void traverse() override;
};

/**
 * new internal type used for annotating built in methods
 */
class _internal : public _expr {
public:
    string klass;
    string method;

    _internal(string c, string m);

    void print(ostream& os) const override;

    void traverse() override{}
};

class _case : public _expr {
public:
    _expr* expr;
    list<_caseElement*> cases;

    _case(int l, _expr* e);

    void print(ostream& os) const override;
    void prettyPrint(ostream& os, string prefix) const;

    void traverse() override;
};







#endif //COOLCOMPILERPROJECTALL_SYNTAXTREENODES_H
