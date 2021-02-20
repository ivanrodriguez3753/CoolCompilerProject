//
// Created by Ivan Rodriguez on 10/19/20.
//

#include "parseTreeNodes.h"


programNode* rootIVAN = nullptr;

programNode::programNode(string gSym, string pb, classListNode *cln) : node{gSym, pb}, clNode{cln} {
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
classListNode::classListNode(string gSym) : node{gSym, ""} {
    for(auto clazz : classList) {
        children->push_back(clazz);
    }
}

featureListNode::featureListNode(string gSym) : node{gSym, ""} {
    for(auto feature : featureList) {
        children->push_back(feature);
    }
}


terminalNode::terminalNode(string gSym) : node{gSym, ""} {
    isTerminal = true;
}


classNode::classNode(string gSym, string pb, terminalNode *tn1, wordNode *wn1, optionalInhNode* optInh, terminalNode *tn2, featureListNode* ftList, terminalNode *tn3, terminalNode *tn4) :
    node{gSym, pb}, CLASS{tn1}, TYPE{wn1}, optionalInh{optInh}, LBRACE{tn2}, featureList{ftList}, RBRACE{tn3}, SEMI{tn4}
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
    node{gSym, ""}, INHERITS{INH}, TYPE{T}
{
    children->push_back(INHERITS);
    children->push_back(TYPE);
}

featureNode::featureNode(string gSym, string pb, wordNode* ID, terminalNode* COL, wordNode* ty, terminalNode* S) :
    node{gSym, pb}, IDENTIFIER{ID}, COLON{COL}, TYPE{ty}, SEMI{S} {
    //let fieldNode/methodNode subclass constructor take care of all this so we can
    //selectively include/exclude optional fields
}




booleanNode::booleanNode(string gSym, bool b) : terminalNode{gSym}, value{b} {

}

exprNode::exprNode(string gSym, string pb) : node{gSym, pb}{

}

fieldNode::fieldNode(string gSym, string pb, wordNode *ID, terminalNode *COL, wordNode *ty, optionalInitNode *in, terminalNode *S) :
    featureNode{gSym, pb, ID, COL, ty, S}, init{in}
{
    children->push_back(IDENTIFIER);
    children->push_back(COLON);
    children->push_back(TYPE);
    if(in != nullptr) children->push_back(in);
    children->push_back(SEMI);
}

optionalInitNode::optionalInitNode(string gSym, terminalNode *l, exprNode *e) :
    node{gSym, ""}, LARROW{l}, expr{e}
{
    children->push_back(l);
    children->push_back(e);
}

methodNode::methodNode(string gSym, string pb, wordNode *ID, terminalNode *LP, formalsListNode *flNode, terminalNode *RP,
                       terminalNode *COL, wordNode *ty, terminalNode *LB, exprNode *exp, terminalNode *RB, terminalNode* S) :
    featureNode{gSym, pb, ID, COL, ty, S}, LPAREN{LP}, formalsList{flNode}, RPAREN{RP}, LBRACE{LB}, expr{exp}, RBRACE{RB}
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

formalNode::formalNode(string gSym, string pb, wordNode *ID, terminalNode *COL, wordNode *TY) :
    node{gSym, pb}, IDENTIFIER{ID}, COLON{COL}, TYPE{TY}
{
    children->push_back(IDENTIFIER);
    children->push_back(COLON);
    children->push_back(TYPE);
}

formalsListNode::formalsListNode(string gSym) :
    node(gSym, "")
{

}

boolExprNode::boolExprNode(string gSym, string pb, booleanNode *b) :
    exprNode{gSym, pb},BOOLEAN{b}
{
    children->push_back(b);
}


assignExprNode::assignExprNode(string gSym, string pb, wordNode* ID, terminalNode* L, exprNode* e) :
    exprNode{gSym, pb}, IDENTIFIER{ID}, LARROW{L}, expr{e}
{
    children->push_back(IDENTIFIER);
    children->push_back(LARROW);
    children->push_back(expr);
}

dispatchNode::dispatchNode(string gSym, string pb, wordNode *ID, terminalNode *LP, exprListNode *el, terminalNode *RP) :
    exprNode{gSym, pb}, IDENTIFIER{ID}, LPAREN{LP}, exprList{el}, RPAREN{RP}
{

}

selfDispatchNode::selfDispatchNode(string gSym, string pb, wordNode *ID, terminalNode *LP, exprListNode *el, terminalNode *RP) :
    dispatchNode(gSym, pb, ID, LP, el, RP)
{
    children->push_back(IDENTIFIER);
    children->push_back(LPAREN);
    children->push_back(exprList);
    children->push_back(RPAREN);
}

dynamicDispatchNode::dynamicDispatchNode(string gSym, string pb, exprNode *e, terminalNode *D, wordNode *ID, terminalNode *LP, exprListNode *el, terminalNode *RP) :
    dispatchNode{gSym, pb, ID, LP, el, RP}, expr{e}, DOT{D}
{
    children->push_back(expr);
    children->push_back(DOT);
    children->push_back(IDENTIFIER);
    children->push_back(LPAREN);
    children->push_back(exprList);
    children->push_back(RPAREN);
}


staticDispatchNode::staticDispatchNode(string gSym, string pb, exprNode *e, terminalNode *A, wordNode *TY, terminalNode *D, wordNode *ID, terminalNode *LP, exprListNode *el, terminalNode *RP) :
    dispatchNode{gSym, pb, ID, LP, el, RP}, expr{e}, AT{A}, TYPE{TY}, DOT{D}
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

exprListNode::exprListNode(string gSym) : node(gSym, "") {

}

ifExprNode::ifExprNode(string gSym, string pb, terminalNode *I, exprNode *pe, terminalNode *TH, exprNode *te, terminalNode *EL, exprNode* ee, terminalNode* F) :
    exprNode{gSym, pb}, IF{I}, predicateExpr{pe}, THEN{TH}, thenExpr{te}, ELSE{EL}, elseExpr{ee}, FI{F}
{
    children->push_back(IF);
    children->push_back(predicateExpr);
    children->push_back(THEN);
    children->push_back(thenExpr);
    children->push_back(ELSE);
    children->push_back(elseExpr);
    children->push_back(FI);
}

whileExprNode::whileExprNode(string gSym, string pb, terminalNode *W, exprNode *pe, terminalNode *L, exprNode *le, terminalNode *P) :
    exprNode{gSym, pb}, WHILE{W}, predicateExpr{pe}, LOOP{L}, loopExpr{le}, POOL{P}
{
    children->push_back(WHILE);
    children->push_back(predicateExpr);
    children->push_back(LOOP);
    children->push_back(loopExpr);
    children->push_back(POOL);
}

blockExprNode::blockExprNode(string gSym, string pb, terminalNode *LB, exprListNode *el, terminalNode *RB) :
    exprNode{gSym, pb}, LBRACE{LB}, exprList{el}, RBRACE{RB}
{
    children->push_back(LBRACE);
    for(auto child : *exprList->children) {
        children->push_back(child);
    }
    children->push_back(RBRACE);
}

letExprNode::letExprNode(string gSym, string pb, terminalNode *L, bindingListNode *bln, terminalNode *I, exprNode *e) :
    exprNode{gSym, pb}, LET{L}, blNode{bln}, IN{I}, expr{e}
{
    children->push_back(LET);
    children->push_back(blNode);
    children->push_back(IN);
    children->push_back(expr);
}

bindingListNode::bindingListNode(string gSym) :
    node{gSym, ""}
{
    for(auto binding : bindingList) {
        children->push_back(binding);
    }
}


bindingNode::bindingNode(string gSym, string pb, wordNode *ID, terminalNode *COL, wordNode *TY, optionalInitNode *i) :
    node{gSym, pb}, IDENTIFIER{ID}, COLON{COL}, TYPE{TY}, init{i}
{
    children->push_back(IDENTIFIER);
    children->push_back(COLON);
    children->push_back(TYPE);
    if(init != nullptr) children->push_back(init);
}

caseNode::caseNode(string gSym, string pb, wordNode *ID, terminalNode *COL, wordNode *TY, terminalNode *RA, exprNode *e, terminalNode *S) :
    node{gSym, pb}, IDENTIFIER{ID}, COLON{COL}, TYPE{TY}, RARROW{RA}, expr{e}, SEMI{S}
{
    children->push_back(IDENTIFIER);
    children->push_back(COLON);
    children->push_back(TYPE);
    children->push_back(RARROW);
    children->push_back(expr);
    children->push_back(SEMI);
}

caseListNode::caseListNode(string gSym) :
    node{gSym, ""}
{
    for(auto kase : caseList) {
        children->push_back(kase);
    }
}

caseExprNode::caseExprNode(string gSym, string pb, terminalNode *C, exprNode *e, terminalNode *O, caseListNode *cln, terminalNode *E) :
    exprNode{gSym, pb}, CASE{C}, expr{e}, OF{O}, clNode{cln}, ESAC{E}
{
    children->push_back(CASE);
    children->push_back(expr);
    children->push_back(OF);
    children->push_back(clNode);
    children->push_back(ESAC);
}

newExprNode::newExprNode(string gSym, string pb, terminalNode* N, wordNode* TY) :
    exprNode{gSym, pb}, NEW{N}, TYPE{TY}
{
    children->push_back(NEW);
    children->push_back(TYPE);
}

isvoidExprNode::isvoidExprNode(string gSym, string pb, terminalNode *IV, exprNode *e) :
    exprNode{gSym, pb}, ISVOID{IV}, expr{e}
{
    children->push_back(ISVOID);
    children->push_back(e);
}

arithExprNode::arithExprNode(string gSym, string pb, exprNode *e1, terminalNode *OP, exprNode *e2) :
    exprNode{gSym, pb}, expr1{e1}, ARITHOP{OP}, expr2{e2}
{
    children->push_back(expr1);
    children->push_back(ARITHOP);
    children->push_back(expr2);
}

relExprNode::relExprNode(string gSym, string pb, exprNode *e1, terminalNode *OP, exprNode *e2) :
    exprNode{gSym, pb}, expr1{e1}, RELOP{OP}, expr2{e2}
{
    children->push_back(expr1);
    children->push_back(RELOP);
    children->push_back(expr2);
}

unaryExprNode::unaryExprNode(string gSym, string pb, terminalNode *OP, exprNode *e) :
    exprNode{gSym, pb}, UNARYOP{OP}, expr{e}
{
    children->push_back(UNARYOP);
    children->push_back(expr);
}

termExprNode::termExprNode(string gSym, string pb, terminalNode *LP, exprNode *e, terminalNode *RP) :
    exprNode{gSym, pb}, LPAREN{LP}, expr{e}, RPAREN{RP}
{
    children->push_back(LPAREN);
    children->push_back(expr);
    children->push_back(RPAREN);
}


identifierExprNode::identifierExprNode(string gSym, string pb, wordNode *ID) :
    exprNode{gSym, pb}, IDENTIFIER{ID}
{
    children->push_back(IDENTIFIER);
}


intExprNode::intExprNode(string gSym, string pb, integerNode *INT) :
    exprNode{gSym, pb}, INTEGER{INT}
{
    children->push_back(INTEGER);
}

stringExprNode::stringExprNode(string gSym, string pb, wordNode *S) :
    exprNode{gSym, pb}, STRING{S}
{
    children->push_back(STRING);
}

integerNode::integerNode(string gSym, int v) :
    terminalNode{gSym}, value{v}
{

}
