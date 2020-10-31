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
    node{gSym}, IDENTIFIER{ID}, COLON{COL}, TYPE{ty}, SEMI{S} {
    //let fieldNode/methodNode subclass constructor take care of all this so we can
    //selectively include/exclude optional fields
}




booleanNode::booleanNode(string gSym, bool b) : terminalNode{gSym}, value{b} {

}

exprNode::exprNode(string gSym) : node{gSym}{
}

fieldNode::fieldNode(string gSym, wordNode *ID, terminalNode *COL, wordNode *ty, optionalInitNode *in, terminalNode *S) :
    featureNode{gSym, ID, COL, ty, S}, init{in}
{
    children->push_back(IDENTIFIER);
    children->push_back(COLON);
    children->push_back(TYPE);
    if(in != nullptr) children->push_back(in);
    children->push_back(SEMI);
}

optionalInitNode::optionalInitNode(string gSym, terminalNode *l, exprNode *e) :
    node{gSym}, LARROW{l}, expr{e}
{
    children->push_back(l);
    children->push_back(e);
}

methodNode::methodNode(string gSym, wordNode *ID, terminalNode *LP, formalsListNode *flNode, terminalNode *RP,
                       terminalNode *COL, wordNode *ty, terminalNode *LB, exprNode *exp, terminalNode *RB, terminalNode* S) :
    featureNode{gSym, ID, COL, ty, S}, LPAREN{LP}, formalsList{flNode}, RPAREN{RP}, LBRACE{LB}, expr{exp}, RBRACE{RB}
{
    children->push_back(IDENTIFIER);
    children->push_back(LPAREN);
    children->push_back(formalsList);
    children->push_back(RPAREN);
    children->push_back(COLON);
    children->push_back(ty);
    children->push_back(LBRACE);
    children->push_back(expr);
    children->push_back(RBRACE);
    children->push_back(SEMI);
}

formalNode::formalNode(string gSym, wordNode *ID, terminalNode *COL, wordNode *TY) :
    node{gSym}, IDENTIFIER{ID}, COLON{COL}, TYPE{TY}
{
    children->push_back(IDENTIFIER);
    children->push_back(COLON);
    children->push_back(TYPE);
}

formalsListNode::formalsListNode(string gSym) :
    node(gSym)
{

}

boolExprNode::boolExprNode(string gSym, booleanNode *b) :
    exprNode{gSym},BOOLEAN{b}
{
    children->push_back(b);
}


assignExprNode::assignExprNode(string gSym, wordNode* ID, terminalNode* L, exprNode* e) :
    exprNode{gSym}, IDENTIFIER{ID}, LARROW{L}, expr{e}
{
    children->push_back(IDENTIFIER);
    children->push_back(LARROW);
    children->push_back(expr);
}

dispatchNode::dispatchNode(string gSym, wordNode *ID, terminalNode *LP, exprListNode *el, terminalNode *RP) :
    exprNode{gSym}, IDENTIFIER{ID}, LPAREN{LP}, exprList{el}, RPAREN{RP}
{

}

selfDispatchNode::selfDispatchNode(string gSym, wordNode *ID, terminalNode *LP, exprListNode *el, terminalNode *RP) :
    dispatchNode(gSym, ID, LP, el, RP)
{
    children->push_back(IDENTIFIER);
    children->push_back(LPAREN);
    children->push_back(exprList);
    children->push_back(RPAREN);
}

dynamicDispatchNode::dynamicDispatchNode(string gSym, exprNode *e, terminalNode *D, wordNode *ID, terminalNode *LP, exprListNode *el, terminalNode *RP) :
    dispatchNode{gSym, ID, LP, el, RP}, expr{e}, DOT{D}
{
    children->push_back(expr);
    children->push_back(DOT);
    children->push_back(IDENTIFIER);
    children->push_back(LPAREN);
    children->push_back(exprList);
    children->push_back(RPAREN);
}


staticDispatchNode::staticDispatchNode(string gSym, exprNode *e, terminalNode *A, wordNode *TY, terminalNode *D, wordNode *ID, terminalNode *LP, exprListNode *el, terminalNode *RP) :
    dispatchNode{gSym, ID, LP, el, RP}, expr{e}, AT{A}, TYPE{TY}, DOT{D}
{
    children->push_back(expr);
    children->push_back(AT);
    children->push_back(TYPE);
    children->push_back(DOT);
    children->push_back(IDENTIFIER);
    children->push_back(LPAREN);
    children->push_back(exprList);
    children->push_back(RPAREN);
}

exprListNode::exprListNode(string gSym) : node(gSym) {

}

ifExprNode::ifExprNode(string gSym, terminalNode *I, exprNode *pe, terminalNode *TH, exprNode *te, terminalNode *EL, exprNode* ee, terminalNode* F) :
    exprNode{gSym}, IF{I}, predicateExpr{pe}, THEN{TH}, thenExpr{te}, ELSE{EL}, elseExpr{ee}, FI{F}
{
    children->push_back(IF);
    children->push_back(predicateExpr);
    children->push_back(THEN);
    children->push_back(thenExpr);
    children->push_back(ELSE);
    children->push_back(elseExpr);
    children->push_back(FI);
}

whileExprNode::whileExprNode(string gSym, terminalNode *W, exprNode *pe, terminalNode *L, exprNode *le, terminalNode *P) :
    exprNode{gSym}, WHILE{W}, predicateExpr{pe}, LOOP{L}, loopExpr{le}, POOL{P}
{
    children->push_back(WHILE);
    children->push_back(predicateExpr);
    children->push_back(LOOP);
    children->push_back(loopExpr);
    children->push_back(POOL);
}

blockExprNode::blockExprNode(string gSym, terminalNode *LB, exprListNode *el, terminalNode *RB) :
    exprNode{gSym}, LBRACE{LB}, exprList{el}, RBRACE{RB}
{
    children->push_back(LBRACE);
    for(auto child : *exprList->children) {
        children->push_back(child);
    }
    children->push_back(RBRACE);
}

letExprNode::letExprNode(string gSym, terminalNode *L, bindingListNode *bln, terminalNode *I, exprNode *e) :
    exprNode{gSym}, LET{L}, blNode{bln}, IN{I}, expr{e}
{
    children->push_back(LET);
    children->push_back(blNode);
    children->push_back(IN);
    children->push_back(expr);
}

bindingListNode::bindingListNode(string gSym) :
    node(gSym)
{
    for(auto binding : bindingList) {
        children->push_back(binding);
    }
}


bindingNode::bindingNode(string gSym, wordNode *ID, terminalNode *COL, wordNode *TY, optionalInitNode *i) :
    node{gSym}, IDENTIFIER{ID}, COLON{COL}, TYPE{TY}, init{i}
{
    children->push_back(IDENTIFIER);
    children->push_back(COLON);
    children->push_back(TYPE);
    if(init != nullptr) children->push_back(init);
}
