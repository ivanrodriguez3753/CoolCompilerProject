#include "ast.h"



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

void _attr::prettyPrint(ostream& os, const string indentPrefix) const {
    cout << indentPrefix + T + "ATTR: " + id + ", " + type << endl;
    //TODO: print initExpr
}

void _method::prettyPrint(ostream& os, const string indentPrefix) const {
    cout << indentPrefix + T + "METHOD: " + id + ", " + returnType << endl;
    cout << indentPrefix + indent + T + "FORMALS_LIST: " + to_string(formalsList.size()) + " formalsList" << endl;
    for(_formal* formal : formalsList) {
        cout << indentPrefix + indent + indent + T + "FORMAL: " + formal->id + ", " + formal->type << endl;
    }
    cout << indentPrefix + indent + T + "METHOD_BODY: XYZ EXPRESSION" << endl;
}