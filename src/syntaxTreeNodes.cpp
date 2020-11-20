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
    top = top->links.at(make_pair(typeIdentifier.identifier, typeIdentifier.kind));
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
    top = top->previous;
}

void _classNoInh::prettyPrint(ostream &os, string prefix) const {
    os << prefix << "├──";
    os << "CLASS:" << typeIdentifier.identifier << endl;

    for(auto feature : featureList) {
        string newPrefix = prefix + "|   ";
        feature->prettyPrint(os, newPrefix);
    }

}

void _classInh::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──";
    os << "CLASS:" << typeIdentifier.identifier << endl;

    os << "|   " <<  "├──INHERITS:" << superClassIdentifier.identifier << endl;

    for(auto feature : featureList) {
        string newPrefix = prefix + "|   ";
        feature->prettyPrint(os, newPrefix);
    }
}

void _attributeInit::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──" << "ATTRIBUTE:";
    os << identifier.identifier << endl;
    string newPrefix = prefix + "|   ";
    expr->prettyPrint(os, newPrefix);
}

void _let::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──" << "LET_EXPR:";
    string newPrefix = prefix + "|   ";
    body->prettyPrint(os, newPrefix);
}

void _node::prettyPrint(ostream& os, string prefix) const {

}

void _attributeNoInit::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──" << "ATTRIBUTE:";
    os << identifier.identifier << endl;
}

void _method::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──" << "METHOD:";
    os << identifier.identifier << endl;
    os << formalList.size() << " FORMALS" << endl;
    string newPrefix = prefix + "├──";
    for(auto formal : formalList) {
        formal->prettyPrint(os, newPrefix);
    }
}

void _formal::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──" << "FORMAL:" << identifier.identifier << endl;
}
void _assign::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──" << "ASSIGN_EXPR" << endl;
    string newPrefix = prefix + "|   ";
    os << prefix << "├──" << identifier.identifier << endl;
    rhs->prettyPrint(os, newPrefix);
}
void _dynamicDispatch::prettyPrint(ostream& os, string prefix) const {
        os << prefix << "├──";
}

void _staticDispatch::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──";
}

void _selfDispatch::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──";
}

void _if::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──";
}
void _while::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──";
}
void _block::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──";
}

void _new::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──";
}

void _isvoid::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──";
}

void _arith::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──";
}

void _relational::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──";
}

void _unary::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──";
}

void _integer::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──";
}

void _string::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──";
}

void _identifier::prettyPrint(ostream& os, string prefix) const {

}

void _bool::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──";
}
void _letBindingNoInit::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──";
}
void _letBindingInit::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──";
}
void _case::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──";
}
void _caseElement::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──";
}


void _classInh::print(ostream &os) const {
    os << typeIdentifier;
    os << "inherits" << endl;
    os << superClassIdentifier;
    os << featureList.size() << endl;
    top = top->links.at(make_pair(typeIdentifier.identifier, typeIdentifier.kind));
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
    top = top->previous;
}



_classInh::_classInh(_idMeta id, _idMeta sId) :
        _class{id}, superClassIdentifier{sId}
{

}

void _idMeta::print(ostream &os) const {
    if(kind != "") {
        os << top->get(make_pair(identifier, kind))->lineNo << endl;
    }else {
        os << lineNo << endl;
    }
    os << identifier << endl;
}

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

void _program::prettyPrint(ostream &os, string prefix) const {
    for(auto klass : classList) {
        klass->prettyPrint(os, "");
    }
}

bool _expr::printExprType = false;
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
    top = top->links.at(make_pair(identifier.identifier, identifier.kind));
    for(_formal* formal : formalList) {
        os << *formal;
    }
    top = top->previous;
    os << typeIdentifier;
    top = top->links.at(make_pair(identifier.identifier, identifier.kind));
    os << *body;
    top = top->previous;

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
    if(printExprType) {
        os << exprType << endl;
    }
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

int _let::letCounter = 0;
_let::_let(int l, _idMeta lk, _expr* b) :
    _expr(l), letKey{lk}, body{b}
{

}

void _let::print(ostream &os) const {
    os << lineNo << endl;
    os << "let" << endl;
    os << bindings.size() << endl;
    top = top->links.at(make_pair(letKey.identifier, letKey.kind));
    for(_letBinding* binding : bindings) {
        os << *binding;
    }
    top = top->previous;
    os << *body;
}

int _caseElement::caseCounter = 0;
_caseElement::_caseElement(_idMeta id, _idMeta typeId, _expr* b, _idMeta ck) :
    _expr{0}, identifier{id}, typeIdentifier{typeId}, body{b}, caseKey{ck}
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
        top = top->links.at(make_pair(Case->caseKey.identifier, "case"));
        os << *Case;
        top = top->previous;
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