#include "syntaxTreeNodes.h"
#include "Environment.h"
#include <iostream>
using namespace std;

_class* Object_class{nullptr};
_class* IO_class{nullptr};
_class* Bool_class{nullptr};
_class* String_class{nullptr};
_class* Int_class{nullptr};



map<string, _class*> basicClassNodes{{"Object", Object_class},
                                     {"IO", IO_class},
                                     {"Bool", Bool_class},
                                     {"String", String_class},
                                     {"Int", Int_class}};



bool _node::isAnnotated; //= false;
_node::_node(int l) : lineNo{l} {

}

ostream &operator<<(ostream &os, const _node &n) {
    n.print(os);
    return os;
}


_class::_class(_idMeta id, classRecord* r) :
    _node{0}, typeIdentifier{id}, rec{r}
{

}

void _class::traverse() {
    top = top->links[{rec->lexeme,rec->kind}];
    for(auto feature : featureList) {
        feature->traverse();
    }
    top = top->previous;
}


_classNoInh::_classNoInh(_idMeta id, classRecord* r) :
        _class{id, r}
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
    string newPrefix = prefix + "|   ";
    os << newPrefix << "├──FEATURE LIST: SIZE IS " << featureList.size() << endl;

    newPrefix += "|   ";
    for(auto feature : featureList) {
        feature->prettyPrint(os, newPrefix);
    }

}

void _classInh::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──";
    os << "CLASS:" << typeIdentifier.identifier << endl;
    string newPrefix = prefix + "|   ";
    os << newPrefix <<  "├──INHERITS:" << superClassIdentifier.identifier << endl;
    os << newPrefix << "├──FEATURE LIST: SIZE IS " << featureList.size() << endl;

    newPrefix += "|   ";
    for(auto feature : featureList) {
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
    os << prefix << "├──" << "LET_EXPR" << endl;
    string newPrefix = prefix + "|   ";
    os << newPrefix << "├──" << "BINDING_LIST: SIZE IS " << bindings.size() << endl;
    string newPrefix2 = newPrefix + "|   ";
    for(auto binding : bindings) {
        binding->prettyPrint(os, newPrefix2);
    }
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
    string newPrefix1 = prefix + "|   ";
    os << newPrefix1 << "├──" << "FORMALS LIST: SIZE IS " << formalList.size() << endl;
    string newPrefix2 = newPrefix1 + "|   ";
    for(auto formal : formalList) {
        formal->prettyPrint(os, newPrefix2);
    }
    body->prettyPrint(os, newPrefix1);
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
    os << prefix << "├──" << "DYNAMIC_DISPATCH_EXPR" << endl;
    string newPrefix = prefix + "|   ";
    os << newPrefix << "├──" << method.identifier << endl;
    os << newPrefix << "├──" << "ARGUMENT LIST: SIZE IS " << args.size() << endl;
    string newPrefix2 = newPrefix + "|   ";
    for(auto arg : args) {
        arg->prettyPrint(os, newPrefix2);
    }
    expr->prettyPrint(os, newPrefix);
}

void _staticDispatch::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──" << "STATIC_DISPATCH_EXPR" << endl;
    string newPrefix = prefix + "|   ";
    os << newPrefix << "├──" << method.identifier << endl;
    os << newPrefix << "├──" << "@" << typeIdentifier.identifier << endl;
    os << newPrefix << "├──" << "ARGUMENT LIST: SIZE IS " << args.size() << endl;
    string newPrefix2 = newPrefix + "|   ";
    for(auto arg : args) {
        arg->prettyPrint(os, newPrefix2);
    }
    expr->prettyPrint(os, newPrefix);

}

void _selfDispatch::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──" << "SELF_DISPATCH_EXPR" << endl;
    string newPrefix = prefix + "|   ";
    os << newPrefix << "├──" << method.identifier << endl;
    os << newPrefix << "├──" << "ARGUMENT LIST: SIZE IS " << args.size() << endl;
    string newPrefix2 = newPrefix + "|   ";
    for(auto arg : args) {
        arg->prettyPrint(os, newPrefix2);
    }
}

void _if::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──" << "IF_EXPRESSION" << endl;
    string newPrefix = prefix + "|   ";
    predicate->prettyPrint(os, newPrefix);
    thenExpr->prettyPrint(os, newPrefix);
    elseExpr->prettyPrint(os, newPrefix);
}
void _while::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──" << "WHILE_EXPRESSION" << endl;
    string newPrefix = prefix + "|   ";
    predicate->prettyPrint(os, newPrefix);
    body->prettyPrint(os, newPrefix);

}
void _block::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──" << "BLOCK_EXPRESSION" << endl;
    string newPrefix = prefix + "|   ";
    os << newPrefix << "├──" << "EXPRESSION LIST: SIZE IS " << body.size() << endl;
    string newPrefix2 = newPrefix + "|   ";
    for(auto expr : body) {
        expr->prettyPrint(os, newPrefix2);
    }

}

void _new::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──" << "NEW_EXPRESSION: " << identifier.identifier << endl;
}

void _isvoid::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──" << "IS_VOID_EXPRESSION" << endl;
    string newPrefix = prefix + "|   ";
    expr->prettyPrint(os, newPrefix);
}

void _arith::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──" << "ARITH_EXPRESSION: " << op << endl;
    string newPrefix = prefix + "|   ";
    left->prettyPrint(os, newPrefix);
    right->prettyPrint(os, newPrefix);
}

void _relational::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──" << "RELATIONAL_EXPRESSION: " << op << endl;
    string newPrefix = prefix + "|   ";
    left->prettyPrint(os, newPrefix);
    right->prettyPrint(os, newPrefix);
}

void _unary::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──" << "UNARY_EXPRESSION: " << op << endl;
    string newPrefix = prefix + "|   ";
    expr->prettyPrint(os, newPrefix);
}

void _integer::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──";
}

void _string::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──" << "STRING_EXPR" << endl;
}

void _identifier::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──" << "IDENTIFIER_EXPR: " << identifier.identifier << endl;

}

void _bool::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──" << "BOOL_EXPR" << endl;
    return;
}
void _letBindingNoInit::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──" << "LET_BINDING_NO_INIT: " << identifier.identifier << endl;
}
void _letBindingInit::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──" << "LET_BINDING_INIT: " << identifier.identifier << endl;
    string newPrefix = prefix + "|   ";
    init->prettyPrint(os, newPrefix);
}
void _case::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──" << "CASE_EXPRESSION" << endl;
    string newPrefix = prefix + "|   ";
    expr->prettyPrint(os, newPrefix);
    os << newPrefix << "CASE LIST: SIZE IS " << cases.size() << endl;
    string newPrefix2 = newPrefix + "|   ";
    for(auto kase : cases) {
        kase->prettyPrint(os, newPrefix2);
    }
}
void _caseElement::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──" << "CASE ELEMENT: " << identifier.identifier << endl;
    string newPrefix = prefix + "|   ";
    body->prettyPrint(os, newPrefix);
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



_classInh::_classInh(_idMeta id, classRecord* r, _idMeta sId) :
        _class{id, r}, superClassIdentifier{sId}
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

/**
 * traverse in a postorder fashion to annotate expression nodes with a type.
 * parents depend on children types. obvious nodes have already been annotated, and we
 * also need base cases for leaves if parents depend on children
 */
void _program::traverse() {
    //top is currently globalEnv
    //have the caller change environments, and follow that convention unless noted otherwise
    for(auto klass : classList) {
        klass->traverse();
    }
    _node::isAnnotated = true;
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

void _attributeInit::traverse() {
    //dont need to traver symTable unless it is an expression that introduces scope (_let or _case I think)
    //so leave it to caller as usual
    expr->traverse();
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

void _method::traverse() {
    top = top->links[{identifier.identifier, "method"}];
    for(auto formal : formalList) {
        formal->traverse();
    }
    body->traverse();
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
    if(isAnnotated) {
        os << exprType << endl;
    }
    os << "block" << endl;
    os << body.size() << endl;
    for(_expr* expr : body) {
        os << *expr;
    }
}

void _block::traverse() {
    for(auto expr : body) {
        expr->traverse();
    }
    exprType = body.back()->exprType; //exprType of a _block expression is defined by the exprType of the last expression in its body
}

_new::_new(int l, _idMeta id) :
    _expr{l}, identifier{id}
{
    exprType = id.identifier;
}

void _new::print(ostream &os) const {
    os << lineNo << endl;
    //TODO: see why isAnnotated is false and not printing out exprType
    if(isAnnotated) {
        os << exprType << endl;
    }
    os << "new" << endl;
    os << identifier;
}

void _new::traverse() {
    exprType = identifier.identifier;
}

_isvoid::_isvoid(int l, _expr *e) :
    _expr{l}, expr{e}
{
    exprType = "Bool";
}

void _isvoid::print(ostream &os) const {
    os << lineNo << endl;
    os << "isvoid" << endl;
    os << *expr;
}

_arith::_arith(int l, _expr *le, string o, _expr *r) :
    _expr{l}, left{le}, op{o}, right{r}
{
    exprType = "Int";
}

void _arith::print(ostream &os) const {
    os << lineNo << endl;
    if(isAnnotated) {
        os << exprType << endl;
    }
    os << op << endl;
    os << *left;
    os << *right;
}

void _arith::traverse() {
    left->traverse();
    right->traverse();
}

_integer::_integer(int l, int v) :
    _expr(l), value{v}
{
    exprType = "Int";
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
    exprType = "Bool";
}

void _relational::print(ostream &os) const {
    os << lineNo << endl;
    if(isAnnotated) {
        os << exprType << endl;
    }
    os << op << endl;
    os << *left;
    os << *right;
}

void _relational::traverse() {
    left->traverse();
    right->traverse();
}

/**
 * The expression ∼ <expr> is the integer complement of <expr>. The subexpression <expr> must have
 * static type Int and the entire expression has static type Int.
 *  The expression not<expr> is the boolean complement of <expr>. The subexpression <expr> must have
 *  static type Bool and the entire expression has static type Bool.
 */
_unary::_unary(int l, string o, _expr* e) :
    _expr{l}, op{o}, expr{e}
{
    if(o == "not") {
        exprType = "Bool";
    }
    else if(o == "negate") {
        exprType = "Int";
    }
}

void _unary::print(ostream& os) const {
    os << lineNo << endl;
    if(isAnnotated) {
        os << exprType << endl;
    }
    os << op << endl;
    os << *expr;
}

void _unary::traverse() {
    expr->traverse();
}

_string::_string(int l, string v) :
    _expr{l}, value{v}
{
    exprType = "String";
}

void _string::print(ostream& os) const {
    os << lineNo << endl;
    if(isAnnotated) {
        os << exprType << endl;
    }
    os << "string" << endl;
    os << value << endl;
}

_identifier::_identifier(int l, _idMeta id) :
    _expr{l}, identifier{id}
{

}

void _identifier::print(ostream& os) const {
    os << lineNo << endl;
    if(isAnnotated) {
        os << exprType << endl;
    }
    os << "identifier" << endl;
    os << identifier;

}

/**
 * can only be an attribute or local variable (which includes formal parameters, or something introduced in a let)
 */
void _identifier::traverse() {
    if(((objectRecord*)top->get(make_pair(identifier.identifier,"attribute")))) {
        exprType = ((objectRecord*)top->get(make_pair(identifier.identifier,"attribute")))->type;
    }
    else if(((objectRecord*)top->get(make_pair(identifier.identifier,"local")))) {
        exprType = ((objectRecord*)top->get(make_pair(identifier.identifier,"local")))->type;
    }
}

_bool::_bool(int l, bool v) :
    _expr{l}, value{v}
{
    exprType = "Bool";
}

void _bool::print(ostream& os) const {
    os << lineNo << endl;
    if(isAnnotated) {
        os << exprType << endl;
    }
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
    if(_node::isAnnotated) {
        os << exprType << endl;
    }
    os << "assign" << endl;
    os << identifier;
    os << *rhs;
}

void _assign::traverse() {
    rhs->traverse();
    exprType = rhs->exprType;
}

_internal::_internal(string c, string m) :
    _expr{0}, klass{c}, method{m}
{
    exprType = "internal";
}

void _internal::print(ostream &os) const {
    os << 0 << endl << exprType << endl << "internal" << endl << klass << "." << method << endl;
}
