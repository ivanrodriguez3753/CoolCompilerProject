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
        os << superId << endl << superLineNo << endl;
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
    //TODO: modify for attribute_init
    os << "attribute_init" << endl;
    os << typeLineNo << endl << type << endl;
    os << lineNo << endl << id << endl;
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
    os << typeLineNo << endl << type << endl;
    os << lineNo << endl << id << endl;
}

void _bool::prettyPrint(ostream &os, const string indentPrefix) const {

}

void _bool::print(ostream& os) const {
    os << lineNo << endl;
    if(value) os << "true" << endl;
    else      os << "false" << endl;
}