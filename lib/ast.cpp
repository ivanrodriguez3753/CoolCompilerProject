#include "ast.h"
#include <algorithm>



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

    //sort features by encountered
    vector<_feature*> features;
    features.insert(features.begin(), featureList.first.begin(), featureList.first.end());
    features.insert(features.end(), featureList.second.begin(), featureList.second.end());
    sort(features.begin(), features.end(), [](const _feature* lhs, const _feature* rhs) {
        return lhs->encountered < rhs->encountered;
    });

    os << features.size() << endl;
    for(_feature* feature : features) {
        if(feature->isAttr) {
            ((_attr*)(feature))->print(os);
        }
        else {
            ((_method*)(feature))->print(os);
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
    if(value) os << "true" << endl;
    else      os << "false" << endl;
}

void _int::print(ostream &os) const {
    os << lineNo << endl;
    os << "integer" << endl;
    os << value << endl;
}

void _selfDispatch::print(ostream &os) const {
    os << lineNo << endl;
    os << "self_dispatch" << endl;

    os << lineNo << endl;
    os << id << endl;

    os << argList.size() << endl;
    for(_expr* expr : argList) {
        expr->print(os);
    }
}

void _dynamicDispatch::print(ostream &os) const {
    os << lineNo << endl;
    os << "dynamic_dispatch" << endl;

    caller->print(os);

    os << lineNo << endl;
    os << id << endl;

    os << argList.size() << endl;
    for(_expr* expr : argList) {
        expr->print(os);
    }
}

void _staticDispatch::print(ostream &os) const {
    os << lineNo << endl;
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

void _id::print(ostream &os) const {
    os << lineNo << endl;
    os << "identifier" << endl;
    os << lineNo << endl << value << endl;
}

void _string::print(ostream &os) const {
    os << lineNo << endl;
    os << "string" << endl;
    os << value << endl;
}

void _if::print(ostream& os) const {
    os << lineNo << endl;
    os << "if" << endl;

    predicate->print(os);
    tthen->print(os);
    eelse->print(os);
}

void _while::print(ostream &os) const {
    os << lineNo << endl;
    os << "while" << endl;

    predicate->print(os);
    body->print(os);
}

void _assign::print(ostream& os) const {
    os << lineNo << endl;
    os << "assign" << endl;

    os << lineNo << endl;
    os << id << endl;

    rhs->print(os);
}

void _block::print(ostream &os) const {
    os << lineNo << endl; //just use line number of first expression
    os << "block" << endl;

    os << body.size() << endl;
    for(_expr* expr : body) {
        expr->print(os);
    }
}

void _new::print(ostream &os) const {
    os << lineNo << endl;
    os << "new" << endl;

    os << typeLineNo << endl;
    os << id << endl;
}

void _isvoid::print(ostream &os) const {
    os << lineNo << endl;
    os << "isvoid" << endl;

    expr->print(os);
}
