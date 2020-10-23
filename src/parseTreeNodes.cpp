//
// Created by Ivan Rodriguez on 10/19/20.
//

#include "parseTreeNodes.h"


programNode* rootIVAN = nullptr;

programNode::programNode(string gSym, classListNode *cln) :node{gSym}, clNode{cln} {
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
classListNode::classListNode(string gSym) : node{gSym} {
    for(auto clazz : classList) {
        children->push_back(clazz);
    }
}

featureListNode::featureListNode(string gSym) : node{gSym} {
    for(auto feature : featureList) {
        children->push_back(feature);
    }
}


terminalNode::terminalNode(string gSym) : node{gSym} {
    isTerminal = true;
}


classNode::classNode(string gSym, terminalNode *tn1, wordNode *wn1, optionalInhNode* optInh, terminalNode *tn2, featureListNode* ftList, terminalNode *tn3, terminalNode *tn4) :
    node{gSym}, CLASS{tn1}, TYPE{wn1}, optionalInh{optInh}, LBRACE{tn2}, featureList{ftList}, RBRACE{tn3}, SEMI{tn4}
{
    children->push_back(CLASS);
    children->push_back(TYPE);
    if(optInh != nullptr) children->push_back(optionalInh);
    children->push_back(LBRACE);
    children->push_back(featureList);
    children->push_back(RBRACE);
    children->push_back(SEMI);
}

wordNode::wordNode(string gSym, string v) :
    terminalNode{gSym} , value{v}
{
}

optionalInhNode::optionalInhNode(string gSym, terminalNode *INH, wordNode *T) :
    node{gSym}, INHERITS{INH}, TYPE{T}
{
    children->push_back(INHERITS);
    children->push_back(TYPE);
}

featureNode::featureNode(string gSym, wordNode* ID, terminalNode* COL, wordNode* ty, terminalNode* S) :
    node{gSym}, IDENTIFIER{ID}, COLON{COL}, TYPE{ty}, SEMI{S}
{
    children->push_back(IDENTIFIER);
    children->push_back(COLON);
    children->push_back(TYPE);
    children->push_back(SEMI);
}

fieldNode::fieldNode(string gSym, wordNode* ID, terminalNode* COL, wordNode* ty, terminalNode* S) :
    featureNode{gSym, ID, COL, ty, S}
{

}
