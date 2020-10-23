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
    list<node*> *children;
    node() : children{new list<node*>()} {}



};

//


/**
* For nodes that don't really carry any information other than the keyword itself
* or punctuation symbol itself, like class or while or : or ) etc
*/
class terminalNode : public node {

public:
    string tokenType;
    terminalNode(string ttype);
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

//for now let's try without possible initialization, and only fields, no methods
class featureNode : public node {
    wordNode* IDENTIFIER;
    terminalNode* COLON;
    wordNode* TYPE;
};

class optionalInhNode : public node {
    terminalNode *INHERITS;
    wordNode *TYPE;

public:
    optionalInhNode(terminalNode *INH, wordNode *T);
};

class featureListNode : node {
    list<featureNode*> featureList;

public:
    featureListNode();
};

class classNode : public node {
    terminalNode *CLASS;
    wordNode *TYPE;
    optionalInhNode *optionalInh;
    terminalNode *LBRACE;
    //featureListNode *featureList;
    terminalNode *RBRACE;
    terminalNode *SEMI;
public:
    classNode(terminalNode* tn1, wordNode* wn1, optionalInhNode* optInh, terminalNode* tn2, terminalNode* tn3, terminalNode* tn4);
};


class classListNode : public node {
public:
    list<classNode*> classList; //not a pointer because inherited children field from node class will contain
                                //what we need to be a pointer, which are the members of this list
    classListNode();
};


class programNode: public node {
protected:
    classListNode *clNode;

public:
    explicit programNode(classListNode* cln);
};

extern programNode* rootIVAN;

#endif //COOLCOMPILERPROJECTALL_PARSETREENODES_H


