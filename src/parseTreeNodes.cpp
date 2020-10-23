//
// Created by Ivan Rodriguez on 10/19/20.
//

#include "parseTreeNodes.h"


programNode* rootIVAN = nullptr;

programNode::programNode(classListNode *cln) : clNode{cln} {
    std::cout << "constructing program node\n";
    children->push_back(clNode);

}

/**
 * In a recursive fashion, the empty rule will be evaluated first
 * So we don't need to do anything, the node must be constructed with
 * an empty list at first. Then the class nodes can be added to the list
 * in their rules since we have a reference to this class list node ($$),
 * which of course has a reference to the list of classes through the
 * classList member which is NOT a pointer.
 */
classListNode::classListNode() {
    cout << "constructing classlist node\n";
    for(auto clazz : classList) {
        children->push_back(clazz);
    }
}

featureListNode::featureListNode() {
    cout << "constructing featurelist node\n";
    for(auto feature : featureList) {
        children->push_back(feature);
    }
}


terminalNode::terminalNode(string ttype) : tokenType{ttype} {
    cout << "constructing terminal node for " << ttype << endl;
}


classNode::classNode(terminalNode *tn1, wordNode *wn1, optionalInhNode* optInh, terminalNode *tn2, terminalNode *tn3, terminalNode *tn4) :
    CLASS{tn1}, TYPE{wn1}, optionalInh{optInh}, LBRACE{tn2}, RBRACE{tn3}, SEMI{tn4}
{
    cout << "Constructing class node\n";
    children->push_back(CLASS);
    children->push_back(TYPE);
    if(optInh != nullptr) children->push_back(optionalInh);
    children->push_back(LBRACE);
    children->push_back(RBRACE);
    children->push_back(SEMI);
}

wordNode::wordNode(string type, string v) :
    terminalNode(type) , value{v}
{
    cout << "constructing word node " << v << endl;
}

optionalInhNode::optionalInhNode(terminalNode *INH, wordNode *T) :
    INHERITS{INH}, TYPE{T}
{
    cout << "constructing optionalInh node" << endl;
    children->push_back(INHERITS);
    children->push_back(TYPE);
}
