/**
 * Used techniques found in the lecture notes at:
 * http://web.eecs.utk.edu/~bvanderz/teaching/cs461Sp11/notes/parse_tree/
 * In particular, we use a list to take care of productions of the form
 * list:
 *      %empty //epsilon
 * |    element list
 * instead of doing the thing where we have several nodes to go down.
 * It's essentially just flattening the list
 */
#ifndef COOLCOMPILERPROJECTALL_PARSETREENODES_H
#define COOLCOMPILERPROJECTALL_PARSETREENODES_H

#include <list>
#include <string>
#include <iostream>
#include "parser.hh"




using namespace std;
//base class, more of a marker
class node {
    /**
     * Instead of just having node members, put them in list because we need a notion
     * of order from left to right.
     */
public:
    bool isTerminal = false;
    string grammarSymbol;
    list<node*> *children;
    node(string gSym) : grammarSymbol{gSym}, children{new list<node*>()} {}



};

//


/**
* For nodes that don't really carry any information other than the keyword itself
* or punctuation symbol itself, like class or while or : or ) etc
*/
class terminalNode : public node {

public:
    terminalNode(string gSym);
};

/**
* For true/false terminals
*/
class booleanNode : public terminalNode {
    bool value;
};

/**
* For integer terminals
*/
class integerNode : terminalNode {
    int value;
};

/**
* For type, identifier, and string terminals
*/
class wordNode : public terminalNode {

public:
    string value;
    wordNode(string type, string v);
};

//for now let's try without possible initialization, and only fields
class featureNode : public node {
    wordNode* IDENTIFIER;
    terminalNode* COLON;
    wordNode* TYPE;
    terminalNode* SEMI;

public:
    featureNode(string gSym, wordNode* ID, terminalNode* COL, wordNode* ty, terminalNode* S);

};

class fieldNode : public featureNode {
    //will add initialization options later
public:
    fieldNode(string gSym, wordNode* ID, terminalNode* COL, wordNode* ty, terminalNode* S);
};

class parameterNode : public node {

};

class parameterListNode : public node {

};

class methodNode : public featureNode {
    terminalNode* LPAREN;
    parameterListNode* parameterList;
    terminalNode* RPAREN;
};



class optionalInhNode : public node {
    terminalNode *INHERITS;
    wordNode *TYPE;

public:
    optionalInhNode(string gSym, terminalNode *INH, wordNode *T);
};

class featureListNode : public node {
public:
    list<featureNode*> featureList;
    featureListNode(string gSym);
};

class classNode : public node {
    terminalNode *CLASS;
    wordNode *TYPE;
    optionalInhNode *optionalInh;
    terminalNode *LBRACE;
    featureListNode *featureList;
    terminalNode *RBRACE;
    terminalNode *SEMI;
public:
    classNode(string gSym, terminalNode* tn1, wordNode* wn1, optionalInhNode* optInh, terminalNode* tn2, featureListNode* ftList, terminalNode* tn3, terminalNode* tn4);
};


class classListNode : public node {
public:
    list<classNode*> classList; //not a pointer because inherited children field from node class will contain
                                //what we need to be a pointer, which are the members of this list
    classListNode(string gSym);
};


class programNode: public node {
protected:
    classListNode *clNode;

public:
    programNode(string gSym, classListNode* cln);
};

extern programNode* rootIVAN;

#endif //COOLCOMPILERPROJECTALL_PARSETREENODES_H


