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
public:
    bool value;
    booleanNode(string gSym, bool b);
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
public:
    wordNode* IDENTIFIER;
    terminalNode* COLON;
    wordNode* TYPE;
    terminalNode* SEMI;

    featureNode(string gSym, wordNode* ID, terminalNode* COL, wordNode* ty, terminalNode* semi);

};

/**
 * For now, just implement the last rule for expr which is
 * expr -> false so we can finish other rules that use an expression
 */
class exprNode : public node {
public:
    exprNode(string gSym);
};

class ifExprNode : public exprNode {
public:
    terminalNode* IF;
    exprNode* predicateExpr;
    terminalNode* THEN;
    exprNode* thenExpr;
    terminalNode* ELSE;
    exprNode* elseExpr;
    terminalNode* FI;
    ifExprNode(string gSym, terminalNode* I, exprNode* pe, terminalNode* TH, exprNode* te, terminalNode* EL, exprNode* ee, terminalNode* F);
};

class whileExprNode : public exprNode {
public:
    terminalNode* WHILE;
    exprNode* predicateExpr;
    terminalNode* LOOP;
    exprNode* loopExpr;
    terminalNode* POOL;
    whileExprNode(string gSym, terminalNode* W, exprNode* pe, terminalNode* L, exprNode* le, terminalNode* P);
};

class boolExprNode : public exprNode {
public:
    booleanNode* BOOLEAN;
    boolExprNode(string gSym, booleanNode* b);
};

class assignExprNode : public exprNode {
public:
    wordNode* IDENTIFIER;
    terminalNode* LARROW;
    exprNode* expr;

    assignExprNode(string gSym, wordNode* ID, terminalNode* L, exprNode* e);
};

class exprListNode : public node {
public:
    list<exprNode*> exprList;
    exprListNode(string gSym);
};

class dispatchNode : public exprNode {
public:
    wordNode* IDENTIFIER;
    terminalNode* LPAREN;
    exprListNode* exprList;
    terminalNode* RPAREN;
    dispatchNode(string gSym, wordNode* ID, terminalNode* LP, exprListNode* el, terminalNode* RP);
};

class selfDispatchNode : public dispatchNode {
public:
    selfDispatchNode(string gSym, wordNode* ID, terminalNode* LP, exprListNode* el, terminalNode* RP);
};

class dynamicDispatchNode : public dispatchNode {
public:
    exprNode* expr;
    terminalNode* DOT;
    dynamicDispatchNode(string gSym, exprNode* e, terminalNode* D, wordNode* ID, terminalNode* LP, exprListNode* el, terminalNode* RP);
};

class staticDispatchNode : public dispatchNode {
public:
    exprNode* expr;
    terminalNode* AT;
    wordNode* TYPE;
    terminalNode* DOT;
    staticDispatchNode(string gSym, exprNode* e, terminalNode* A, wordNode* TY, terminalNode* D, wordNode* ID, terminalNode* LP, exprListNode* el, terminalNode* RP);

};

class optionalInitNode : public node {
public:
    terminalNode* LARROW;
    exprNode* expr;
    optionalInitNode(string gSym, terminalNode* l, exprNode* e);
};

class fieldNode : public featureNode {
public:
    optionalInitNode* init;
    fieldNode(string gSym, wordNode* ID, terminalNode* COL, wordNode* ty, optionalInitNode* in, terminalNode* S);
};


class formalNode : public node {
public:
    wordNode* IDENTIFIER;
    terminalNode* COLON;
    wordNode* TYPE;
    formalNode(string gSym, wordNode* ID, terminalNode* COL, wordNode* TY);
};

class formalsListNode : public node {
public:
    list<formalNode*> formalsList;
    formalsListNode(string gSym);
};


class methodNode : public featureNode {
public:
    terminalNode* LPAREN;
    formalsListNode* formalsList;
    terminalNode* RPAREN;
    terminalNode* LBRACE;
    exprNode* expr;
    terminalNode* RBRACE;

    methodNode(string gSym, wordNode *ID, terminalNode *LP, formalsListNode *flNode, terminalNode *RP,
               terminalNode *COL, wordNode *ty, terminalNode *LB, exprNode *exp, terminalNode *RB, terminalNode* S);
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


