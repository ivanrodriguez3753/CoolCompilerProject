#include "syntaxTreeNodes.h"
#include "Environment.h"
#include "type.h"
#include <iostream>
using namespace std;

_class* Object_class{nullptr};
_class* IO_class{nullptr};
_class* Bool_class{nullptr};
_class* String_class{nullptr};
_class* Int_class{nullptr};

vector<pair<int,string>> errorLog{};

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
    os << prefix << "├──" << "LET_EXPR<$type=" << exprType << ", $lineNo=" << lineNo << ">" << endl;
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
    os << prefix << "├──" << "ASSIGN_EXPR<$type=" << exprType << ", $lineNo=" << lineNo << ">" << endl;
    string newPrefix = prefix + "|   ";
    os << newPrefix << "├──" << identifier.identifier << endl;
    rhs->prettyPrint(os, newPrefix);
}
void _dynamicDispatch::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──" << "DYNAMIC_DISPATCH_EXPR<$type=" << exprType << ", $lineNo=" << lineNo << ">" << endl;
    string newPrefix = prefix + "|   ";
    os << newPrefix << "├──" << method.identifier << endl;
    os << newPrefix << "├──" << "ARGUMENT LIST: SIZE IS " << args.size() << endl;
    string newPrefix2 = newPrefix + "|   ";
    for(auto arg : args) {
        arg->prettyPrint(os, newPrefix2);
    }
    caller->prettyPrint(os, newPrefix);
}

void _staticDispatch::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──" << "STATIC_DISPATCH_EXPR<$type=" << exprType << ", $lineNo=" << lineNo << ">" << endl;
    string newPrefix = prefix + "|   ";
    os << newPrefix << "├──" << method.identifier << endl;
    os << newPrefix << "├──" << "@" << typeIdentifier.identifier << endl;
    os << newPrefix << "├──" << "ARGUMENT LIST: SIZE IS " << args.size() << endl;
    string newPrefix2 = newPrefix + "|   ";
    for(auto arg : args) {
        arg->prettyPrint(os, newPrefix2);
    }
    caller->prettyPrint(os, newPrefix);

}

void _selfDispatch::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──" << "SELF_DISPATCH_EXPR<$type=" << exprType << ", $lineNo=" << lineNo << ">" << endl;
    string newPrefix = prefix + "|   ";
    os << newPrefix << "├──" << method.identifier << endl;
    os << newPrefix << "├──" << "ARGUMENT LIST: SIZE IS " << args.size() << endl;
    string newPrefix2 = newPrefix + "|   ";
    for(auto arg : args) {
        arg->prettyPrint(os, newPrefix2);
    }
}

void _if::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──" << "IF_EXPRESSION<$type=" << exprType << ", $lineNo=" << lineNo << ">" << endl;
    string newPrefix = prefix + "|   ";
    predicate->prettyPrint(os, newPrefix);
    thenExpr->prettyPrint(os, newPrefix);
    elseExpr->prettyPrint(os, newPrefix);
}
void _while::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──" << "WHILE_EXPRESSION<$type=" << exprType << ", $lineNo=" << lineNo << ">" << endl;
    string newPrefix = prefix + "|   ";
    predicate->prettyPrint(os, newPrefix);
    body->prettyPrint(os, newPrefix);

}
void _block::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──" << "BLOCK_EXPRESSION<$type=" << exprType << ", $lineNo=" << lineNo << ">" << endl;
    string newPrefix = prefix + "|   ";
    os << newPrefix << "├──" << "EXPRESSION LIST: SIZE IS " << body.size() << endl;
    string newPrefix2 = newPrefix + "|   ";
    for(auto expr : body) {
        expr->prettyPrint(os, newPrefix2);
    }

}

void _new::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──" << "NEW_EXPRESSION<$type=" << exprType << ", $lineNo=" << lineNo << ">" << identifier.identifier << endl;
}

void _isvoid::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──" << "IS_VOID_EXPRESSION<$type=" << exprType << ", $lineNo=" << lineNo << ">" << endl;
    string newPrefix = prefix + "|   ";
    expr->prettyPrint(os, newPrefix);
}

void _arith::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──" << "ARITH_EXPRESSION<$type=" << exprType << ", $lineNo=" << lineNo << ">" << op << endl;
    string newPrefix = prefix + "|   ";
    left->prettyPrint(os, newPrefix);
    right->prettyPrint(os, newPrefix);
}

void _relational::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──" << "RELATIONAL_EXPRESSION<$type=" << exprType << ", $lineNo=" << lineNo << ">" << op << endl;
    string newPrefix = prefix + "|   ";
    left->prettyPrint(os, newPrefix);
    right->prettyPrint(os, newPrefix);
}

void _unary::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──" << "UNARY_EXPRESSION<$type=" << exprType << ", $lineNo=" << lineNo << ">" << op << endl;
    string newPrefix = prefix + "|   ";
    expr->prettyPrint(os, newPrefix);
}

void _integer::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──";
}

void _string::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──" << "STRING_EXPR<$type=" << exprType << ", $lineNo=" << lineNo << ">" << endl;
}

void _identifier::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──" << "IDENTIFIER_EXPR<$type=" << exprType << ", $lineNo=" << lineNo << ">" << identifier.identifier << endl;

}

void _bool::prettyPrint(ostream& os, string prefix) const {
    os << prefix << "├──" << "BOOL_EXPR<$type=" << exprType << ", $lineNo=" << lineNo << ">" << endl;
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
    os << prefix << "├──" << "CASE_EXPRESSION<$type=" << exprType << ", $lineNo=" << lineNo << ">" << endl;
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


void _classInh::typeCheck() {
    //CLASS-2
    try {
        if(superClassIdentifier.identifier == "String" || superClassIdentifier.identifier == "Int" || superClassIdentifier.identifier == "Bool") {
            throw pair<int, string>{superClassIdentifier.lineNo, "class " + typeIdentifier.identifier + " inherits from " + superClassIdentifier.identifier};
        }
    }
    catch (pair<int, string> error){
        cerr << "ERROR: " << to_string(error.first) << ", " << error.second << endl;
        errorLog.push_back(error);
    }
}
_classInh::_classInh(_idMeta id, classRecord* r, _idMeta sId) :
        _class{id, r}, superClassIdentifier{sId}
{
    typeCheck();
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

void _program::typeCheck() {
    //CLASS-1
    try {
        globalEnv->links.at({"Main", "class"});
        //METHOD-1
        try {
            int numFormals_main = ((methodRecord*)globalEnv->links.at({"Main", "class"})->symTable.at({"main", "method"}))->treeNode->formalList.size();
            if(numFormals_main) {
                throw exception();
            }
        }
        catch(...) {
            pair<int, string> error{0, "Didn't define a 0-param main method inside Main class\n"};
            cerr << "ERROR: " << to_string(error.first) << ", " << error.second << endl;
            errorLog.push_back(error);
        }
    } catch (...){
        pair<int, string> error{0, "Didn't define a Main class!\n"};
        cerr << "ERROR: " << to_string(error.first) << ", " << error.second << endl;
        errorLog.push_back(error);
    }
}
/**
 * traverse in a postorder fashion to annotate expression nodes with a type.
 * parents depend on children types. obvious nodes have already been annotated, and we
 * also need base cases for leaves if parents depend on children
 */
void _program::traverse() {
    for(auto klass : classList) {
        klass->traverse();
    }
    _node::isAnnotated = true;

    typeCheck();
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

void _attributeInit::traverse() {
    //dont need to traverse symTable unless it is an expression that introduces scope (_let or _case I think)
    //so leave it to caller as usual
    expr->traverse();
}


_method::_method(_idMeta id, _idMeta typeId, _expr *b) :
        _feature{0, id, typeId}, body{b}
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
string lookUpSelfType(Environment* current) {
    while(current->metaInfo.kind != "class") {
        current = current->previous;
    }
    return current->metaInfo.identifier;
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
        _dispatch{l, m}, caller{e}
{

}

void _dynamicDispatch::print(ostream& os) const {
    os << lineNo << endl;
    if(isAnnotated) {
        os << exprType << endl;
    }
    os << "dynamic_dispatch" << endl;
    os << *caller;
    os << method;
    os << args.size() << endl;
    for(_expr* arg : args) {
        os << *arg;
    }
}

void _dynamicDispatch::traverse() {
    for(_expr* arg : args) {
        arg->traverse();
    }
    caller->traverse();

    /**
     * If f has return type B and B is a class name, then the static type of the dispatch is B. Otherwise, if f has
     * return type SELF_TYPE, then the static type of the dispatch is A. To see why this is sound, note that the self
     * parameter of the method f conforms to type A. Therefore, because f returns SELF_TYPE, we can infer that the
     * result must also conform to A. Inferring accurate static types for dispatch expressions is what justifies
     * including SELF_TYPE in the Cool type system.
     */
    if(caller->exprType == "SELF_TYPE") { //TODO this branch is a bandaid?
        exprType = "SELF_TYPE";
    }
    else if(implementationMap.at(caller->exprType).at(method.identifier).first->returnType != "SELF_TYPE") { //static type of dispatch is B, since it is a class name
        exprType = implementationMap.at(caller->exprType).at(method.identifier).first->returnType;
    }
    else if(implementationMap.at(caller->exprType).at(method.identifier).first->returnType == "SELF_TYPE") {//static type of dispatch is A,
        exprType = caller->exprType;
    }
}

_staticDispatch::_staticDispatch(int l, _idMeta m, _expr* e, _idMeta ty) :
        _dispatch(l, m), caller{e}, typeIdentifier{ty}
{

}

void _staticDispatch::print(ostream& os) const {
    os << lineNo << endl;
    if(isAnnotated) {
        os << exprType << endl;
    }
    os << "static_dispatch" << endl;
    os << *caller;
    os << typeIdentifier;
    os << method;
    os << args.size() << endl;
    for(_expr* arg : args) {
        os << *arg;
    }
}

void _staticDispatch::traverse() {
    for(_expr* arg : args) {
        arg->traverse();
    }
    caller->traverse();
    //lookup method method.identifier in the implementation map for caller->exprType
    //To be clear, you are calling what WOULD be called if the caller was type @RHSType
    //So it doesn't have to have an original (re)definition by RHSType, and caller's
    // static type just has to conform to RHSType
    //value of _staticDispatch is value returned by the method selected (most recent method in hierarchy with that name)
        //but i guess it wouldn't matter which one we choose if all we're assigning is type, since overridden methods
        //have to keep the same return type, same number of arguments and their types
    exprType = implementationMap.at(typeIdentifier.identifier).at(method.identifier).first->returnType;

}

_selfDispatch::_selfDispatch(int l, _idMeta m) :
    _dispatch{l, m}
{

}

void _selfDispatch::print(ostream& os) const {
    os << lineNo << endl;
    if(isAnnotated) {
        os << exprType << endl;
    }
    os << "self_dispatch" << endl;
    os << method;
    os << args.size() << endl;
    for(_expr* arg : args) {
        os << *arg;
    }
}

void _selfDispatch::traverse() {
    for(_expr* arg : args) {
        arg->traverse();
    }

    //TODO improve this but for now go up the chains until you get to a "class" environment, then check its entry in implementationMap
    Environment* current = top;
    while(current->metaInfo.kind != "class") current = current->previous;
    //current is the classEnv
    exprType = implementationMap.at(current->metaInfo.identifier).at(method.identifier).first->returnType;
}

_if::_if(int l, _expr* p, _expr* te, _expr* ee) :
    _expr{l}, predicate{p}, thenExpr{te}, elseExpr{ee}
{

}

void _if::print(ostream& os) const {
    os << lineNo << endl;
    if(isAnnotated) {
        os << exprType << endl;
    }
    os << "if" << endl;
    os << *predicate;
    os << *thenExpr;
    os << *elseExpr;
}

void _if::typeCheck() {
    //EXPRESSION-1
    try {
        if(predicate->exprType != "Bool") {
            throw pair<int, string>{lineNo, "conditional has type " + predicate->exprType + " instead of Bool"};
        }
    }
    catch(pair<int, string> error) {
        cerr << "ERROR: " << to_string(error.first) << ", " << error.second << endl;
        errorLog.push_back(error);
    }
}

void _if::traverse() {
    predicate->traverse();
    thenExpr->traverse();
    elseExpr->traverse();

    exprType = getLub(vector<string>{thenExpr->exprType, elseExpr->exprType});

    typeCheck();
}

_while::_while(int l, _expr* p, _expr* b) :
    _expr{l}, predicate{p}, body{b}
{
    //"The static type of a loop expression is Object"
    exprType = "Object";
}

void _while::print(ostream& os) const {
    os << lineNo << endl;
    if(isAnnotated) {
        os << exprType << endl;
    }
    os << "while" << endl;
    os << *predicate;
    os << *body;
}

void _while::typeCheck() {
    //EXPRESSION-2
    try {
        if(predicate->exprType != "Bool") {
            throw pair<int, string>{lineNo, "conditional has type " + predicate->exprType + " instead of Bool"};
        }
    }
    catch(pair<int, string> error) {
        cerr << "ERROR: " << to_string(error.first) << ", " << error.second << endl;
        errorLog.push_back(error);
    }
}

void _while::traverse() {
    predicate->traverse();
    body->traverse();

    typeCheck();
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
    if(isAnnotated) {
        os << exprType << endl;
    }
    os << "isvoid" << endl;
    os << *expr;
}

void _isvoid::traverse() {
    expr->traverse();
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
void _arith::typeCheck() {
    try {
        if(left->exprType != "Int" || right->exprType != "Int") {
            throw pair<int,string>{lineNo, "Cannot do arithmetic operation " + op + " on " + left->exprType + " and " + right->exprType + ", only on two Int's\n"};
        }
    }
    catch (pair<int, string> error){
        cerr << "ERROR: " << to_string(error.first) << ", " << error.second << endl;
        errorLog.push_back(error);
    }
}
void _arith::traverse() {
    left->traverse();
    right->traverse();

    typeCheck();
}

_integer::_integer(int l, int v) :
    _expr(l), value{v}
{
    exprType = "Int";
}

void _integer::print(ostream& os) const {
    os << lineNo << endl;
    if(isAnnotated) {
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
    //need to account for identifiers from an earlier class in the hierarchy, so find the class we're in
    Environment* current = top;
    while(current->metaInfo.kind != "class") current = current->previous;
    vector<string>inhPath = getInheritancePath(current->metaInfo.identifier);
    for(string klass : inhPath) {
        Environment* classEnv = globalEnv->links.at({klass, "class"});
        if(((objectRecord*)top->get(make_pair(identifier.identifier,"local")))) {
            exprType = ((objectRecord*)top->get(make_pair(identifier.identifier,"local")))->type;
            return;
        }
        else if((classEnv->symTable.find(make_pair(identifier.identifier,"attribute"))) != classEnv->symTable.end()) {
            exprType = ((objectRecord*)classEnv->symTable.at(make_pair(identifier.identifier,"attribute")))->type;
            return;
        }
    }
    cout << "DIDN'T MATCH A BRANCH IN _IDENTIFIER_EXPR\n";
    exit(1);
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

void _letBindingInit::traverse() {
    init->traverse();
}

int _let::letCounter = 0;
_let::_let(int l, _idMeta lk, _expr* b) :
    _expr(l), letKey{lk}, body{b}
{

}

void _let::print(ostream &os) const {
    os << lineNo << endl;
    if(isAnnotated) {
        os << exprType << endl;
    }
    os << "let" << endl;
    os << bindings.size() << endl;
    top = top->links.at(make_pair(letKey.identifier, letKey.kind));
    for(_letBinding* binding : bindings) {
        os << *binding;
    }
    os << *body;
    top = top->previous;
}

void _let::traverse() {
    for(auto binding : bindings) {
        //Don't need to change scope for a binding w/o initialization but will do it anyway
        top = top->links.at({letKey.identifier, letKey.kind});
        binding->traverse();
        top = top->previous;
    }
    top = top->links.at({letKey.identifier, letKey.kind});
    body->traverse();
    top = top->previous;

    exprType = body->exprType;
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
    if(isAnnotated) {
        os << exprType << endl;
    }
    os << "case" << endl;
    os << *expr;
    os << cases.size() << endl;
    for(_caseElement* Case : cases) {
        top = top->links.at(make_pair(Case->caseKey.identifier, Case->caseKey.kind));
        os << *Case;
        top = top->previous;
    }
}

void _case::traverse() {
    expr->traverse();
    vector<string> typeChoices; //I believe all these must conform expr->exprType
    for(auto kase : cases) {
        top = top->links.at({kase->caseKey.identifier, "case"});
        kase->traverse();
        typeChoices.push_back(kase->exprType);
        top = top->previous;
    }
    //go up the hierarchy tree as much as we can
    exprType = getLub(typeChoices);

}


void _caseElement::print(ostream &os) const {
    os << identifier;
    os << typeIdentifier;
    os << *body;
}

void _caseElement::traverse() {
    body->traverse();
    exprType = body->exprType;
}

_assign::_assign(int l, _idMeta id, _expr* r) :
    _expr{l}, identifier{id}, rhs{r}
{

}

void _assign::print(ostream& os) const {
    os << lineNo << endl;
    if(isAnnotated) {
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
    os << 0 << endl;
    if(isAnnotated) {
        os<< exprType << endl;
    }
    os << "internal" << endl << klass << "." << method << endl;
}
