#include "syntaxTreeNodes.h"
#include <iostream>
using namespace std;


_node::_node(int l) : lineNo{l} {

}

ostream &operator<<(ostream &os, const _node &n) {
    n.print(os);
    return os;
}


_class::_class(_idMeta id) :
    _node{0}, typeIdentifier{id}
{

}

_classNoInh::_classNoInh(_idMeta id) :
        _class{id}
{
}

void _classNoInh::print(ostream &os) const {
    os << typeIdentifier;
    os << "no_inherits" << endl;
    os << featureList.size() << endl;
    for(auto feature : featureList) {
        if((_attributeNoInit*) feature) {
            os << *(_attributeNoInit*)feature;
        }
        else if((_attributeInit*) feature) {
            os << *(_attributeInit*)feature;
        }
        else if((_method*) feature) {
            os << *(_method*)feature;
        }
    }
}

void _classInh::print(ostream &os) const {
    os << typeIdentifier;
    os << "inherits" << endl;
    os << superClassIdentifier;
    os << featureList.size() << endl;
    for(auto feature : featureList) {
        if((_attributeNoInit*) feature) {
            os << *(_attributeNoInit*)feature;
        }
    }
}

//ostream &operator<<(ostream &os, const _classNode &c) {
//    os << c.typeIdentifier;
//    os << "no_inherits" << endl;
//    os << c.featureList.size() << endl;
//    for(auto feature : c.featureList) {
//        if((_attributeNoInit*) feature) {
//            os << *(_attributeNoInit*)feature;
//        }
//
//    }
//    return os;
//    c.print(os);
//    return os;
//}



_classInh::_classInh(_idMeta id, _idMeta sId) :
        _class{id}, superClassIdentifier{sId}
{

}


//ostream& operator<<(ostream& os, const _identifier& i) {
//    os << i.lineNo << endl;
//    os << i.identifier << endl;
//    return os;
//}

void _idMeta::print(ostream &os) const {
    os << lineNo << endl;
    os << identifier << endl;
}

//ostream &operator<<(ostream &os, const _programNode &p) {
//    os << p.classList.size() << endl;
//    for(auto klass : p.classList) {
//        os << *klass;
//    }
//    return os;
//}

//ostream &operator<<(ostream &os, const _attributeNoInit& a) {
//    os << "attribute_no_init" << endl;
//    os << a.identifier;
//    os << a.typeIdentifier;
//    return os;
//}

_feature::_feature(int l, _idMeta id, _idMeta typeId) :
    _node{l}, identifier{id}, typeIdentifier{typeId}
{

}

_attributeNoInit::_attributeNoInit(int l, _idMeta id, _idMeta typeId) :
        _feature{l, id, typeId}
{

}

void _attributeNoInit::print(ostream &os) const {
    os << "attribute_no_init" << endl;
    os << identifier;
    os << typeIdentifier;
}

void _program::print(ostream &os) const {
    os << classList.size() << endl;
    for(auto klass : classList) {
        os << *klass;
    }
}


_expr::_expr(int l) : _node(l) {

}

_attributeInit::_attributeInit(int l, _idMeta id, _idMeta typeId, _expr *e) :
        _feature{l, id, typeId}, expr{e}
{

}

void _attributeInit::print(ostream &os) const {
    os << "attribute_init" << endl;
    os << identifier;
    os << typeIdentifier;
    os << *expr;
}

_method::_method(_idMeta id, _idMeta typeId, _expr *e) :
        _feature{0, id, typeId}, body{e}
{

}

void _method::print(ostream &os) const {
    os << "method" << endl;
    os << identifier;
    os << formalList.size() << endl;
    for(_formal* formal : formalList) {
        os << *formal;
    }
    os << typeIdentifier;
    os << *body;
}

_formal::_formal(_idMeta id, _idMeta typeId) :
    _node{0}, identifier{id}, typeIdentifier{typeId}
{

}

void _formal::print(ostream &os) const {
    os << identifier;
    os << typeIdentifier;
}

_dispatch::_dispatch(int l, _idMeta m) :
    _expr{l}, method{m}
{

}

_dynamicDispatch::_dynamicDispatch(int l, _idMeta m, _expr *e) :
    _dispatch{l, m}, expr{e}
{

}

void _dynamicDispatch::print(ostream& os) const {
    os << lineNo << endl;
    os << "dynamic_dispatch" << endl;
    os << *expr;
    os << method;
    os << args.size() << endl;
    for(_expr* arg : args) {
        os << *arg;
    }
}

_staticDispatch::_staticDispatch(int l, _idMeta m, _expr* e, _idMeta ty) :
    _dispatch(l, m), expr{e}, typeIdentifier{ty}
{

}

void _staticDispatch::print(ostream& os) const {
    os << lineNo << endl;
    os << "static_dispatch" << endl;
    os << *expr;
    os << typeIdentifier;
    os << method;
    os << args.size() << endl;
    for(_expr* arg : args) {
        os << *arg;
    }
}

_selfDispatch::_selfDispatch(int l, _idMeta m) :
    _dispatch{l, m}
{

}

void _selfDispatch::print(ostream& os) const {
    os << lineNo << endl;
    os << "self_dispatch" << endl;
    os << method;
    os << args.size() << endl;
    for(_expr* arg : args) {
        os << *arg;
    }
}

_if::_if(int l, _expr* p, _expr* te, _expr* ee) :
    _expr{l}, predicate{p}, thenExpr{te}, elseExpr{ee}
{

}

void _if::print(ostream& os) const {
    os << lineNo << endl;
    os << "if" << endl;
    os << *predicate;
    os << *thenExpr;
    os << *elseExpr;
}

_while::_while(int l, _expr* p, _expr* b) :
    _expr{l}, predicate{p}, body{b}
{

}

void _while::print(ostream& os) const {
    os << lineNo << endl;
    os << "while" << endl;
    os << *predicate;
    os << *body;
}

_block::_block(int l) :
    _expr{l}
{

}

void _block::print(ostream& os) const {
    os << lineNo << endl;
    os << "block" << endl;
    os << body.size() << endl;
    for(_expr* expr : body) {
        os << *expr;
    }
}

_new::_new(int l, _idMeta id) :
    _expr{l}, identifier{id}
{

}

void _new::print(ostream &os) const {
    os << lineNo << endl;
    os << "new" << endl;
    os << identifier;
}

_isvoid::_isvoid(int l, _expr *e) :
    _expr{l}, expr{e}
{

}

void _isvoid::print(ostream &os) const {
    os << lineNo << endl;
    os << "isvoid" << endl;
    os << *expr;
}

_arith::_arith(int l, _expr *le, string o, _expr *r) :
    _expr{l}, left{le}, op{o}, right{r}
{

}

void _arith::print(ostream &os) const {
    os << lineNo << endl;
    os << op << endl;
    os << *left;
    os << *right;
}

_integer::_integer(int l, int v) :
    _expr(l), value{v}
{

}

void _integer::print(ostream& os) const {
    os << lineNo << endl;
    os << "integer" << endl;
    os << value << endl;
}

_relational::_relational(int l, _expr *le, string o, _expr *r) :
    _expr{l}, left{le}, op{o}, right{r}
{

}

void _relational::print(ostream &os) const {
    os << lineNo << endl;
    os << op << endl;
    os << *left;
    os << *right;
}

_unary::_unary(int l, string o, _expr* e) :
    _expr{l}, op{o}, expr{e}
{

}

void _unary::print(ostream& os) const {
    os << lineNo << endl;
    os << op << endl;
    os << *expr;
}

_string::_string(int l, string v) :
    _expr{l}, value{v}
{

}

void _string::print(ostream& os) const {
    os << lineNo << endl;
    os << "string" << endl;
    os << value << endl;
}

_identifier::_identifier(int l, _idMeta id) :
    _expr{l}, identifier{id}
{

}

void _identifier::print(ostream& os) const {
    os << lineNo << endl;
    os << "identifier" << endl;
    os << identifier;

}

_bool::_bool(int l, bool v) :
    _expr{l}, value{v}
{

}

void _bool::print(ostream& os) const {
    os << lineNo << endl;
    if(value) {
        os << "true" << endl;
    }
    else {
        os << "false" << endl;
    }
}

_letBinding::_letBinding(_idMeta id, _idMeta typeId) :
    _expr{0}, identifier{id}, typeIdentifier{typeId}
{

}

_letBindingNoInit::_letBindingNoInit(_idMeta id, _idMeta typeId) :
    _letBinding{id, typeId}
{

}

void _letBindingNoInit::print(ostream& os) const {
    os << "let_binding_no_init" << endl;
    os << identifier;
    os << typeIdentifier;
}

_letBindingInit::_letBindingInit(_idMeta id, _idMeta typeId, _expr* i) :
    _letBinding(id, typeId), init{i}
{

}

void _letBindingInit::print(ostream& os) const {
    os << "let_binding_init" << endl;
    os << identifier;
    os << typeIdentifier;
    os << *init;
}

_let::_let(int l, _expr* b) :
    _expr(l), body{b}
{

}

void _let::print(ostream &os) const {
    os << lineNo << endl;
    os << "let" << endl;
    os << bindings.size() << endl;
    for(_letBinding* binding : bindings) {
        os << *binding;
    }
    os << *body;
}

_caseElement::_caseElement(_idMeta id, _idMeta typeId, _expr* b) :
    _expr{0}, identifier{id}, typeIdentifier{typeId}, body{b}
{

}

_case::_case(int l, _expr* e) :
    _expr{l}, expr{e}
{

}

void _case::print(ostream &os) const {
    os << lineNo << endl;
    os << "case" << endl;
    os << *expr;
    os << cases.size() << endl;
    for(_caseElement* Case : cases) {
        os << *Case;
    }
}



void _caseElement::print(ostream &os) const {
    os << identifier;
    os << typeIdentifier;
    os << *body;
}

_assign::_assign(int l, _idMeta id, _expr* r) :
    _expr{l}, identifier{id}, rhs{r}
{

}

void _assign::print(ostream& os) const {
    os << lineNo << endl;
    os << "assign" << endl;
    os << identifier;
    os << *rhs;
}