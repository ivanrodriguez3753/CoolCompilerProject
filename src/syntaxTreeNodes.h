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


class _node {
public:
    int lineNo;
    _node(int l);

    virtual void print(ostream& os) const = 0;

    friend ostream& operator<<(ostream& os, const _node& n);
};

class _idMeta : public _node{
public:
    string identifier;
    string kind;

    _idMeta(int l, string id, string k = "") : _node{l}, identifier{id}, kind{k} {}

    //friend ostream& operator<<(ostream& os, const _identifier& i);
    void print(ostream &os) const override;
};

/**
 * A Cool program is merely a list of classes
 */
class _program : public _node {
public:
    list<_class*> classList;
    _program(int l) : _node{l} {}


    void print(ostream &os) const override;
//    friend ostream& operator<<(ostream& os, const _programNode& p);
};

class _class : public _node {
public:
    _idMeta typeIdentifier;
    list<_feature*> featureList;


    //friend ostream& operator<<(ostream& os, const _classNode& c);
    _class(_idMeta id);
};

class _classNoInh : public _class {
public:

    _classNoInh(_idMeta id);

    void print(ostream& os) const override;
};

class _classInh : public _class {
public:
    _idMeta superClassIdentifier;

    _classInh(_idMeta id, _idMeta sId);
    void print(ostream& os) const override;
};

class _feature : public _node {
public:
    _idMeta identifier;
    _idMeta typeIdentifier;

    _feature(int l, _idMeta id, _idMeta typeId);

};

class _attributeNoInit : public _feature {
public:
    _attributeNoInit(int l, _idMeta id, _idMeta typeId);

    //friend ostream& operator<<(ostream& os, const _attributeNoInit& a);
    void print(ostream& os) const override;
};



class _attributeInit : public _feature {
public:
    _expr* expr;

    _attributeInit(int l, _idMeta id, _idMeta typeId, _expr* e);

    void print(ostream& os) const override;
};

class _method : public _feature {
public:
    list<_formal*> formalList;
    _expr* body;

    _method(_idMeta id, _idMeta typeId, _expr* e);

    void print(ostream& os) const override;
};

class _formal : public _node {
public:
    _idMeta identifier;
    _idMeta typeIdentifier;

    void print(ostream &os) const override;

    _formal(_idMeta id, _idMeta typeId);
};

class _expr : public _node {
public:
    _expr(int l);
};

class _assign : public _expr {
public:
    _idMeta identifier;
    _expr* rhs;

    _assign(int l, _idMeta id, _expr* r);

    void print(ostream& os) const override;
};

class _dispatch : public _expr {
public:
    _idMeta method;
    list<_expr*> args;

    _dispatch(int l, _idMeta m);
};

class _dynamicDispatch : public _dispatch {
public:
    _expr* expr;

    _dynamicDispatch(int l, _idMeta m, _expr* e);

    void print(ostream& os) const override;
};

class _staticDispatch : public _dispatch {
public:
    _expr* expr;
    _idMeta typeIdentifier;

    _staticDispatch(int l, _idMeta m, _expr* e, _idMeta ty);

    void print(ostream& os) const override;
};

class _selfDispatch : public _dispatch {
public:
    _selfDispatch(int l, _idMeta m);

    void print(ostream& os) const override;
};

class _if : public _expr {
public:
    _expr* predicate;
    _expr* thenExpr;
    _expr* elseExpr;

    _if(int l, _expr* p, _expr* te, _expr* ee);

    void print(ostream& os) const override;
};

class _while : public _expr {
public:
    _expr* predicate;
    _expr* body;

    _while(int l, _expr* p, _expr* b);

    void print(ostream& os) const override;
};

class _block : public _expr {
public:
    list<_expr*> body;

    _block(int l);

    void print(ostream& os) const override;
};

class _new : public _expr {
public:
    _idMeta identifier;

    _new(int l, _idMeta id);

    void print(ostream& os) const override;
};

class _isvoid : public _expr {
public:
    _expr* expr;

    _isvoid(int l, _expr* e);

    void print(ostream& os) const override;
};

class _arith : public _expr {
public:
    _expr* left;
    string op; //+ - * / but in words
    _expr* right;

    _arith(int l, _expr* le, string o, _expr* r);

    void print(ostream& os) const override;
};

class _relational : public _expr {
public:
    _expr* left;
    string op; // < <= - but with letters: lt le eq
    _expr* right;

    _relational(int l, _expr* le, string o, _expr* r);

    void print(ostream& os) const override;
};

class _unary : public _expr {
public:
    string op; //not or ~, using words not or negate
    _expr* expr;

    _unary(int l, string o, _expr* e);

    void print(ostream& os) const override;
};

class _integer : public _expr {
public:
    int value;

    _integer(int l, int v);

    void print(ostream& os) const override;
};

class _string : public _expr {
public:
    string value;

    _string(int l, string v);

    void print(ostream& os) const override;
};

class _identifier : public _expr {
public:
    _idMeta identifier;

    _identifier(int l, _idMeta id);

    void print(ostream& os) const override;
};

class _bool : public _expr {
public:
    bool value;

    _bool(int l, bool v);

    void print(ostream& os) const override;
};

class _letBinding : public _expr {
public:
    _idMeta identifier;
    _idMeta typeIdentifier;

    _expr* body;

    _letBinding(_idMeta id, _idMeta typeId);
};

class _letBindingNoInit : public _letBinding {
public:
    _letBindingNoInit(_idMeta id, _idMeta typeId);

    void print(ostream& os) const override;
};

class _letBindingInit : public _letBinding {
public:
    _expr* init;

    _letBindingInit(_idMeta id, _idMeta typeId, _expr* i);

    void print(ostream& os) const override;
};

class _let : public _expr {
public:
    list<_letBinding*> bindings;
    _expr* body;

    _let(int l, _expr* b);

    void print(ostream& os) const override;
};

class _caseElement : public _expr {
public:
    _idMeta identifier;
    _idMeta typeIdentifier;
    _expr* body;

    _caseElement(_idMeta id, _idMeta typeId, _expr* b);

    void print(ostream& os) const override;
};

class _case : public _expr {
public:
    _expr* expr;
    list<_caseElement*> cases;

    _case(int l, _expr* e);

    void print(ostream& os) const override;
};







#endif //COOLCOMPILERPROJECTALL_SYNTAXTREENODES_H
