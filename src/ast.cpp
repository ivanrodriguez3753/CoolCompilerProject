#include "ast.h"
#include <algorithm>
#include <set>
#include "ParserDriver.hh"



void _node::print(ostream& os) const {
    cout << "called _node::print\n";
}

const string _node::T = "├──";
const string _node::indent = "|   ";
void _node::prettyPrint(ostream&os, string indentPrefix) const {
    //TODO
    cout << indentPrefix + T + "SHOULDN'T BE CALLED: PLACEHOLDER, MAKE PURE VIRTUAL LATER" << endl;
}

void _program::prettyPrint(ostream& os, const string indentPrefix) const {
    //TODO: give information on which thread parsed this portion of the program (a subset of the global class list)
    cout << indentPrefix + T + "PROGRAM:THREAD_NUM" << endl;

    for(_class* klass : classList) {
        klass->prettyPrint(os, indentPrefix + indent);
    }
}

void _program::print(ostream &os) const {
    os << classList.size() << endl;
    for(_class* klass : classList) {
        klass->print(os);
    }
}

void _class::prettyPrint(ostream& os, const string indentPrefix) const {
    cout << indentPrefix + T + "CLASS: " + id << endl;
    if(superId != "Object") {
        cout << indentPrefix + indent + T + "INHERITS:" + superId << endl;
    }
    cout << indentPrefix + indent + T + "FEATURE_LIST: " + to_string(featureList.first.size()) + " attributes, "
        + to_string(featureList.second.size()) + " methods" << endl;

    //TODO print in the order encountered instead of all attributes followed by all methods
    for(_attr* attr : featureList.first) {
        attr->prettyPrint(os, indentPrefix + indent + indent);
    }
    for(_method* method : featureList.second) {
        method->prettyPrint(os, indentPrefix + indent + indent);
    }
}

void _class::print(ostream &os) const {
    os << lineNo << endl << id << endl;

    if(superId == "Object") {
        os << "no_inherits" << endl;
    } else {
        os << "inherits" << endl;
        os << superLineNo << endl << superId << endl;
    }

    //sort features by encountered, but keep information about whether we are attr/method
    vector<pair<_feature*, bool>> features;
    for(_attr* attr : featureList.first) {
        features.push_back(make_pair(attr, true));
    }
    for(_method* method : featureList.second) {
        features.push_back(make_pair(method, false));
    }
    sort(features.begin(), features.end(), [](const pair<_feature*, bool> lhs, const pair<_feature*, bool> rhs) {
        return lhs.first->encountered < rhs.first->encountered;
    });

    os << features.size() << endl;
    for(pair<_feature*, bool> feature : features) {
        if(feature.second) {
            ((_attr*)(feature.first))->print(os);
        }
        else {
            ((_method*)(feature.first))->print(os);
        }
    }
}

void _attr::prettyPrint(ostream& os, const string indentPrefix) const {
    cout << indentPrefix + T + "ATTR: " + id + ", " + type << endl;
    //TODO: print initExpr
}

void _attr::print(ostream &os) const {
    if(optInit) os << "attribute_init" << endl;
    else os << "attribute_no_init" << endl;
    os << lineNo << endl << id << endl;
    os << typeLineNo << endl << type << endl;

    if(optInit) {
        optInit->print(os);
    }
}

void _method::prettyPrint(ostream& os, const string indentPrefix) const {
    os << indentPrefix + T + "METHOD: " + id + ", " + returnType << endl;
    os << indentPrefix + indent + T + "FORMALS_LIST: " + to_string(formalsList.size()) + " formalsList" << endl;
    for(_formal* formal : formalsList) {
        os << indentPrefix + indent + indent + T + "FORMAL: " + formal->id + ", " + formal->type << endl;
    }
    os << indentPrefix + indent + T + "METHOD_BODY: XYZ EXPRESSION" << endl;
}

void _method::print(ostream &os) const {
    os << "method" << endl;
    os << lineNo << endl << id << endl;

    os << formalsList.size() << endl;
    for(_formal* formal : formalsList) {
        formal->print(os);
    }

    os << typeLineNo << endl << returnType << endl;

    body->print(os);
}

void _formal::prettyPrint(ostream&os, const string indentPrefix) const {

}

void _formal::print(ostream &os) const {
    os << lineNo << endl << id << endl;
    os << typeLineNo << endl << type << endl;
}

void _bool::prettyPrint(ostream &os, const string indentPrefix) const {

}

void _bool::print(ostream& os) const {
    os << lineNo << endl;
    if(isDecorated) os << type << endl;
    if(value) os << "true" << endl;
    else      os << "false" << endl;
}

void _int::print(ostream &os) const {
    os << lineNo << endl;
    if(isDecorated) os << type << endl;
    os << "integer" << endl;
    os << value << endl;
}

void _selfDispatch::print(ostream &os) const {
    os << lineNo << endl;
    if(isDecorated) os << type << endl;
    os << "self_dispatch" << endl;

    os << lineNo << endl;
    os << id << endl;

    os << argList.size() << endl;
    for(_expr* expr : argList) {
        expr->print(os);
    }
}

void _selfDispatch::decorate(ParserDriver &drv) {
    for(_expr* arg : argList) {
        arg->decorate(drv);
    }

    //SELF_TYPE resolution isn't needed since only implementationMap expects resolved SELF_TYPEs
    type = drv.implementationMap.at(drv.currentClassEnv->id).at(id).first->returnType;
    isDecorated = true;
}

void _dynamicDispatch::print(ostream &os) const {
    os << caller->lineNo << endl;
    if(isDecorated) os << type << endl;
    os << "dynamic_dispatch" << endl;

    caller->print(os);

    os << lineNo << endl;
    os << id << endl;

    os << argList.size() << endl;
    for(_expr* expr : argList) {
        expr->print(os);
    }
}

void _dynamicDispatch::decorate(ParserDriver &drv) {
    for(_expr* arg : argList) {
        arg->decorate(drv);
    }
    caller->decorate(drv);

    string trueType;

    string callerType = caller->type;
    string callerTypeResolved = callerType;
    //resolve the caller's type, in case it is SELF_TYPE
    if(callerTypeResolved == "SELF_TYPE") callerTypeResolved = drv.currentClassEnv->id;

    string methodReturnType = drv.implementationMap.at(callerTypeResolved).at(id).first->returnType;
    if(methodReturnType == "SELF_TYPE") {
        type = callerType; //we want to be able to return SELF_TYPE. we just needed to resolve to search implementationMap
    }
    else if(methodReturnType != "SELF_TYPE") {//TODO document this section. using else if to be clear, but after documentation just use else
        type = methodReturnType;
    }
    isDecorated = true;
}

void _staticDispatch::print(ostream &os) const {
    os << lineNo << endl;
    if(isDecorated) os << type << endl;
    os << "static_dispatch" << endl;

    caller->print(os);


    os << typeLineNo << endl;
    os << staticType << endl;

    os << lineNo << endl;
    os << id << endl;

    os << argList.size() << endl;
    for(_expr* expr : argList) {
        expr->print(os);
    }
}

void _staticDispatch::decorate(ParserDriver &drv) {
    for(_expr* arg : argList) {
        arg->decorate(drv);
    }

    caller->decorate(drv);

    //resolve SELF_TYPE if necessary (someExpr@SELF_TYPE.methodName())
    string resolvedAtType = staticType;
    if(resolvedAtType == "SELF_TYPE") resolvedAtType = drv.currentClassEnv->id;
    type = drv.implementationMap.at(resolvedAtType).at(id).first->returnType;
    isDecorated = true;
}

void _id::print(ostream &os) const {
    os << lineNo << endl;
    if(isDecorated) os << type << endl;
    os << "identifier" << endl;
    os << lineNo << endl << value << endl;
}


/**
 * always check letCaseEnv, then methodEnv, then classEnv
 * @param drv
 */
void _id::decorate(ParserDriver &drv) {
    objRec* obj;


    bool foundId = false;

    letCaseEnv *letCaseCurrent = drv.top;
    while ((letCaseCurrent != nullptr) && !foundId) {
        obj = letCaseCurrent->getRec(value);
        if(obj) foundId = true;
        letCaseCurrent = letCaseCurrent->prevLetCase;
    }

    //method parameters (methodEnv)
    if(!foundId) { //don't search method parameters if we are initializing attributes
        obj = drv.currentMethodEnv->getRec(value);
        if(obj) foundId = true;
    }

    //attributes (classEnv, and all parents classEnv's)
    if(!foundId) {

        classEnv* localCurrentClassEnv = drv.currentClassEnv;
        while(localCurrentClassEnv != nullptr) {
            obj = localCurrentClassEnv->getRec(value);
            if(obj) {
                foundId = true;
                break;
            }
            //update localCurrentClassEnv as to traverse the inheritance chain
            //need to go all the way up, but Object doesn't have attributes so we don't need to check it
            string nextParent = drv.env->getRec(localCurrentClassEnv->id)->parent;
            if(nextParent != "Object") {
                localCurrentClassEnv = drv.env->getRec(nextParent)->link;
            }
            else {
                localCurrentClassEnv = nullptr;
            }
        }
    }

    type = obj->type;
    isDecorated = true;
}

void _string::print(ostream &os) const {
    os << lineNo << endl;
    if(isDecorated) os << type << endl;
    os << "string" << endl;
    os << value << endl;
}

void _string::decorate(ParserDriver &drv) {
    type = "String";
    isDecorated = true;

    if(drv.strLits.find(value) == drv.strLits.end()) {
        llvmKey =  drv.strLits.size();
        string llvmName = ".str." + to_string(llvmKey);
        drv.strLits.insert({value, {llvmKey, drv.llvmBuilder->CreateGlobalStringPtr(value, llvmName, 0, drv.llvmModule)}});
    }
    else {
        llvmKey = drv.strLits.at(value).first;
    }
}

void _if::print(ostream& os) const {
    os << lineNo << endl;
    if(isDecorated) os << type << endl;
    os << "if" << endl;

    predicate->print(os);
    tthen->print(os);
    eelse->print(os);
}

void _if::decorate(ParserDriver &drv) {
    predicate->decorate(drv);
    tthen->decorate(drv);
    eelse->decorate(drv);

    set<string> staticTypes{tthen->type, eelse->type};

    type = drv.computeLub(staticTypes);
    isDecorated = true;
}

void _while::print(ostream &os) const {
    os << lineNo << endl;
    if(isDecorated) os << type << endl;
    os << "while" << endl;

    predicate->print(os);
    body->print(os);
}

void _while::decorate(ParserDriver &drv) {
    predicate->decorate(drv);
    body->decorate(drv);

    type = "Object"; //defined in the Cool Reference Manual
    isDecorated = true;
}

void _assign::print(ostream& os) const {
    os << lineNo << endl;
    if(isDecorated) os << type << endl;
    os << "assign" << endl;

    os << lineNo << endl;
    os << id << endl;

    rhs->print(os);
}

void _block::print(ostream &os) const {
    os << lineNo << endl; //just use line number of first expression
    if(isDecorated) os << type << endl;
    os << "block" << endl;

    os << body.size() << endl;
    for(_expr* expr : body) {
        expr->print(os);
    }
}

void _new::print(ostream &os) const {
    os << lineNo << endl;
    if(isDecorated) os << type << endl;
    os << "new" << endl;

    os << typeLineNo << endl;
    os << id << endl;
}

void _new::decorate(ParserDriver &drv) {
    type = id;
    isDecorated = true;
}

void _isvoid::print(ostream &os) const {
    os << lineNo << endl;
    if(isDecorated) os << type << endl;
    os << "isvoid" << endl;

    expr->print(os);
}

void _isvoid::decorate(ParserDriver &drv) {
    expr->decorate(drv);

    type = "Bool";
    isDecorated = true;
}

void _arith::print(ostream &os) const {
    os << lineNo << endl;
    if(isDecorated) os << type << endl;

    if(OP == OPS::PLUS) os << "plus" <<  endl;
    else if(OP == OPS::MINUS) os << "minus" <<  endl;
    else if(OP == OPS::TIMES) os << "times" <<  endl;
    else if(OP == OPS::DIVIDE) os << "divide" <<  endl;

    lhs->print(os);
    rhs->print(os);
}

void _arith::decorate(ParserDriver &drv) {
    lhs->decorate(drv);
    rhs->decorate(drv);

    type = "Int";
    isDecorated = true;
}

void _relational::print(ostream& os) const {
    os << lineNo << endl;
    if(isDecorated) os << type << endl;

    if(OP == OPS::LT) os << "lt" << endl;
    else if(OP == OPS::LE) os << "le" << endl;
    else if(OP == OPS::EQUALS) os << "eq" << endl;

    lhs->print(os);
    rhs->print(os);
}

void _relational::decorate(ParserDriver &drv) {
    lhs->decorate(drv);
    rhs->decorate(drv);

    type = "Bool";
    isDecorated = true;
}

void _unary::print(ostream& os) const {
    os << lineNo << endl;
    if(isDecorated) os << type << endl;

    if(OP == 0) os << "not" << endl;
    else if(OP == 1) os << "negate" << endl;

    expr->print(os);
}

void _unary::decorate(ParserDriver &drv) {
    expr->decorate(drv);

    if(OP == OPS::NEG) type = "Int";
    else if(OP == OPS::NOT) type = "Bool";

    isDecorated = true;
}

void _let::print(ostream& os) const {
    os << lineNo << endl;
    if(isDecorated) os << type << endl;

    os << "let" << endl;
    os << bindingList.size() << endl;
    for(_letBinding* letBinding : bindingList) {
        letBinding->print(os);
    }

    body->print(os);
}

void _letBinding::print(ostream& os) const {
    if(!optInit) os << "let_binding_no_init" << endl;
    else os << "let_binding_init" << endl;

    os << lineNo << endl << id << endl;
    os << type_lineno << endl << type << endl;

    if(optInit) optInit->print(os);


}

void _caseElement::print(ostream &os) const {
    os << lineNo << endl << id << endl;
    os << type_lineno << endl << type << endl;

    caseBranch->print(os);
}

void _case::print(ostream &os) const {
    os << lineNo << endl;
    if(isDecorated) os << type << endl;
    os << "case" << endl;

    switchee->print(os);

    os << caseList.size() << endl;
    for(_caseElement* caseElement : caseList) {
        caseElement->print(os);
    }
}

void _internal::print(ostream &os) const {
    os << lineNo << endl;
    if(isDecorated) os << type << endl;
    os << "internal" << endl;
    os << classDotMethod << endl;
}

void _internal::decorate(ParserDriver &drv) {
    //taken care of in _program::decorateInternals
}

void _program::decorateInternals(env* env) {
    //for every method, delete the _expr* body then new up an _internal _expr
    for(_class* klass : classList) {
        for(_method* method : klass->featureList.second) {
            delete method->body;
            method->body = new _internal(klass->id + '.' + method->id);
            method->body->type = method->returnType;
            method->body->isDecorated = true;
        }
    }
}

void _program::decorate(ParserDriver& drv) {
    for(_class* klass : classList) {
        drv.currentClassEnv = drv.env->getRec(klass->id)->link;
        klass->decorate(drv);
        drv.currentClassEnv = nullptr;
    }
}

void _class::decorate(ParserDriver& drv) {
    assemblyConstructorEnv = new methodEnv(nullptr, "");

    //TODO use class map for this instead of traversing links
    //set up all attributes including inherited attributes.
    classEnv* currentTraverseInh = drv.currentClassEnv;
    while(currentTraverseInh != nullptr) {
        assemblyConstructorEnv->symTable.insert(currentTraverseInh->symTable.begin(), currentTraverseInh->symTable.end());

        //update currentTraverseInh
        string nextParent = drv.env->getRec(currentTraverseInh->id)->parent;
        if(nextParent != "Object") currentTraverseInh = drv.env->getRec(nextParent)->link;
        else currentTraverseInh = nullptr;
    }

    assemblyConstructorEnv->symTable.insert({"self", new objRec(nullptr, 0, -1, "SELF_TYPE")});
    //the 0 and -1 parameters are more or less unimportant

    drv.currentMethodEnv = assemblyConstructorEnv;
    decorateAttrInitExprs(drv);
    drv.currentMethodEnv = nullptr;

    for(_method* method : featureList.second) {
        drv.currentMethodEnv = drv.currentClassEnv->getMethodRec(method->id)->link;
        method->body->decorate(drv);
        drv.currentMethodEnv = nullptr;
    }
}

void _method::decorate(ParserDriver& drv) {
    body->decorate(drv);
}

void _bool::decorate(ParserDriver& drv) {
    type = "Bool";
    isDecorated = true;
}

void _assign::decorate(ParserDriver& drv) {
    rhs->decorate(drv);
    type = rhs->type;
    isDecorated = true;
}

void _let::decorate(ParserDriver& drv) {
    letCaseEnv* letEnv = drv.buildLetEnv(this);

    //traverse the initializers, at the current scope
    for(_letBinding* binding : bindingList) {
        if(binding->optInit) binding->optInit->decorate(drv);
    }

    //traversing the body, which is the first block that can use the new identifiers,
    //needs to be done with the proper context. so set drv.top to the letCaseEnv we just created
    drv.top = letEnv;
    body->decorate(drv);
    drv.top = drv.top->prevLetCase;

    type = body->type;
    isDecorated = true;
}

void _letBinding::decorate(ParserDriver& drv) {
    if(optInit) optInit->decorate(drv);
}

void _int::decorate(ParserDriver& drv) {
    type = "Int";
    isDecorated = true;
}

void _block::decorate(ParserDriver& drv) {
    for(_expr* expr : body) {
        expr->decorate(drv);
    }

    type = body.back()->type;
    isDecorated = true;
}

void _case::decorate(ParserDriver& drv) {
    vector<letCaseEnv*> caseEnvs = drv.buildCaseEnvs(this);

    switchee->decorate(drv);

    set<string> choices;
    for(int i = 0; i < caseList.size(); ++i) {
        _caseElement* caseElem = caseList[i];

        drv.top = caseEnvs[i];
        caseElem->decorate(drv);

        choices.insert(caseElem->caseBranch->type);

        drv.top = drv.top->prevLetCase;
    }


    type = drv.computeLub(choices);

    isDecorated = true;
}

void _caseElement::decorate(ParserDriver& drv) {
    caseBranch->decorate(drv);
}