
#include "ParserDriver.hh"
#include "parser.hh"
#include "parseTreeNodes.h"
#include "type.h"
ParserDriver::ParserDriver() : trace_parsing{false}, trace_scanning{false} {}

int ParserDriver::parse(const std::string& f) {
    file = f;
    location.initialize(&file);
    scan_begin();
    yy::parser parse(*this);
    parse.set_debug_level(trace_parsing);
    int res = parse();
    scan_end();
    return res;
}

/**
 * Driver method for testing the parse tree. Prints the input file by traversing the parse tree
 * and printing leaf nodes (terminals).
 */
void ParserDriver::postorderTraversal(ostream& out) {
    postorderRecurs(rootIVAN, out);
}

void ParserDriver::postorderRecurs(node *current, ostream& out) {
    for(auto child : *current->children) {
        postorderRecurs(child, out);
    }
    if(current->children->empty()) { //node with no children
        if(current->isTerminal) { //we need this because we can have empty lists, which we don't want to print anything for
            terminalNode *term = (terminalNode *) current;
            if (term->grammarSymbol == "identifier" || term->grammarSymbol == "type" || term->grammarSymbol == "string") {
                wordNode *word = (wordNode *) term;
                out << word->value << endl;
            }
            else if(term->grammarSymbol == "integer") {
                integerNode *integer = (integerNode*) term;
                out << integer->value << endl;
            }
            else {
                if(tokenReqTranslation.find(term->grammarSymbol) != tokenReqTranslation.end()) {
                    out << tokenReqTranslation[term->grammarSymbol] << endl;
                }
                else {
                    out << term->grammarSymbol << endl;
                }
            }
        }
    }

}

//taken and adapted from https://stackoverflow.com/questions/36802354/print-binary-tree-in-a-pretty-way-using-c
void ParserDriver::prettyPrintTree(ostream& out) {
    prettyPrintRecurs(rootIVAN, "", out);
}

//uses preorder traversal strategy
void ParserDriver::prettyPrintRecurs(node* current, const string& prefix, ostream& out) {
    if(current != nullptr) {
        out << prefix;
        out << "├──";

        //print the node
        out << current->grammarSymbol << endl;

        //enter the next tree level
        for(auto child : *current->children) {
            string newPrefix = prefix + "|   ";
            prettyPrintRecurs(child, newPrefix, out);
        }


    }
}

/**
 * Do a postorder traversal of the parse tree to construct the abstract syntax tree
 * @param root
 * @return
 */
_program *ParserDriver::buildSyntaxTree(programNode *root) {
    _program* ast = (_program*) buildSyntaxNode(root);
    return ast;
}

/**
 * Builds abstract syntax tree and starts populating symbol table
 * @param current
 * @return
 */
_node *ParserDriver::buildSyntaxNode(node* current) {
    string syntaxNodeType = current->productionBody;
    if(syntaxNodeType == "program") {
        programNode* castedCurrent = (programNode*) current;
        _program* result = new _program{castedCurrent->lineNo};
        for(classNode* klass : castedCurrent->clNode->classList) {
            //TODO
            if(klass->optionalInh == nullptr) {
                top->install(make_pair(klass->TYPE->value,"class"),  new classRecord{klass->TYPE->value, klass->TYPE->lineNo, "class", "Object"});
            }
            else {
                top->install(make_pair(klass->TYPE->value,"class"), new classRecord{klass->TYPE->value, klass->TYPE->lineNo, "class", klass->optionalInh->TYPE->value});
            }
            top->links.insert(make_pair(make_pair(klass->TYPE->value, "class"), new Environment{top}));
            top = top->links.at(make_pair(klass->TYPE->value, "class"));
            result->classList.push_back((_class*)buildSyntaxNode(klass));
            top = top->previous;
        }
        return result;
    }
    else if(syntaxNodeType == "no_inherits") {
        classNode* castedCurrent = (classNode*) current;

        Environment* temp = top;
        top = top->previous;
        classRecord* rec = (classRecord*)top->symTable.at(make_pair(castedCurrent->TYPE->value, "class"));
        top = temp;

        _classNoInh* result = new _classNoInh{_idMeta{castedCurrent->TYPE->lineNo, castedCurrent->TYPE->value, "class"}, rec };
        rec->treeNode = result;
        for(featureNode* feature : castedCurrent->featureList->featureList) {
            result->featureList.push_back((_feature*)buildSyntaxNode(feature));
        }
        return result;
    }
    else if(syntaxNodeType == "inherits") {
        classNode* castedCurrent = (classNode*) current;

        Environment* temp = top;
        top = top->previous;
        classRecord* rec = (classRecord*)top->symTable.at(make_pair(castedCurrent->TYPE->value, "class"));
        top = temp;

        _classInh* result = new _classInh{_idMeta{castedCurrent->TYPE->lineNo, castedCurrent->TYPE->value, "class"},
                                          rec,
                                          _idMeta{castedCurrent->optionalInh->TYPE->lineNo, castedCurrent->optionalInh->TYPE->value}};
        rec->treeNode = result;
        for(featureNode* feature : castedCurrent->featureList->featureList) {
            result->featureList.push_back((_feature*)buildSyntaxNode(feature));
        }
        return result;
    }
    else if(syntaxNodeType == "attribute_no_init") {
        fieldNode* castedCurrent = (fieldNode*) current;
        top->install(make_pair(castedCurrent->IDENTIFIER->value, "attribute"), new objectRecord{castedCurrent->IDENTIFIER->value, castedCurrent->IDENTIFIER->lineNo, "attribute", castedCurrent->TYPE->value, nullptr});
        _attributeNoInit* result = new _attributeNoInit{castedCurrent->lineNo, _idMeta{castedCurrent->IDENTIFIER->lineNo, castedCurrent->IDENTIFIER->value, "attribute"}, _idMeta{castedCurrent->TYPE->lineNo, castedCurrent->TYPE->value}};
        return result;
    }
    else if(syntaxNodeType == "attribute_init") {
        fieldNode* castedCurrent = (fieldNode*) current;
        _attributeInit* result = new _attributeInit{
                castedCurrent->lineNo,
                _idMeta{castedCurrent->IDENTIFIER->lineNo, castedCurrent->IDENTIFIER->value, "attribute"},
                _idMeta{castedCurrent->TYPE->lineNo, castedCurrent->TYPE->value},
                (_expr*)buildSyntaxNode(castedCurrent->init->expr)
        };
        top->install(make_pair(castedCurrent->IDENTIFIER->value, "attribute"), new objectRecord{castedCurrent->IDENTIFIER->value, castedCurrent->IDENTIFIER->lineNo, "attribute", castedCurrent->TYPE->value, result->expr});
        return result;
    }
    else if(syntaxNodeType == "method") {
        methodNode* castedCurrent = (methodNode*) current;
        //expression may introduce new scopes (with the let or case expressions)
        top->install(make_pair(castedCurrent->IDENTIFIER->value, "method"), new Record{castedCurrent->IDENTIFIER->value, castedCurrent->IDENTIFIER->lineNo, "method"});
        top->links.insert(make_pair(make_pair(castedCurrent->IDENTIFIER->value, "method"), new Environment{top}));
        top = top->links.at(make_pair(castedCurrent->IDENTIFIER->value, "method"));
        _method* result = new _method{
                _idMeta{castedCurrent->IDENTIFIER->lineNo, castedCurrent->IDENTIFIER->value, "method"},
                _idMeta{castedCurrent->TYPE->lineNo, castedCurrent->TYPE->value},
                (_expr*)buildSyntaxNode(castedCurrent->expr)
        };
        for(formalNode* formal : castedCurrent->formalsList->formalsList) {
            top->install(make_pair(formal->IDENTIFIER->value, "local"), new Record{formal->IDENTIFIER->value, formal->IDENTIFIER->lineNo, "local"});
            result->formalList.push_back((_formal*)buildSyntaxNode(formal));
        }
        top = top->previous;
        return result;
    }
    else if(syntaxNodeType == "formal") {
        formalNode* castedCurrent = (formalNode*) current;
        _formal* result = new _formal{
                _idMeta{castedCurrent->IDENTIFIER->lineNo, castedCurrent->IDENTIFIER->value, "local"},
                _idMeta{castedCurrent->TYPE->lineNo, castedCurrent->TYPE->value}
        };
        return result;
    }
    else if(syntaxNodeType == "assign") {
        assignExprNode* castedCurrent = (assignExprNode*)current;
        _assign* result = new _assign {
            castedCurrent->lineNo,
            _idMeta{castedCurrent->IDENTIFIER->lineNo, castedCurrent->IDENTIFIER->value},
            (_expr*)buildSyntaxNode(castedCurrent->expr)
        };
        return result;
    }
    else if(syntaxNodeType == "dynamic_dispatch") {
        dynamicDispatchNode* castedCurrent = (dynamicDispatchNode*) current;
        _dynamicDispatch* result = new _dynamicDispatch{
                castedCurrent->lineNo,
                _idMeta{castedCurrent->IDENTIFIER->lineNo, castedCurrent->IDENTIFIER->value},
                (_expr*)buildSyntaxNode(castedCurrent->expr)
        };
        for(exprNode* arg : castedCurrent->exprList->exprList) {
            result->args.push_back((_expr*) buildSyntaxNode(arg));
        }
        return result;
    }
    else if(syntaxNodeType == "static_dispatch") {
        staticDispatchNode *castedCurrent = (staticDispatchNode *) current;
        _staticDispatch *result = new _staticDispatch{
                castedCurrent->lineNo,
                _idMeta{castedCurrent->IDENTIFIER->lineNo, castedCurrent->IDENTIFIER->value},
                (_expr *) buildSyntaxNode(castedCurrent->expr),
                _idMeta{castedCurrent->TYPE->lineNo, castedCurrent->TYPE->value}
        };
        for (exprNode *arg : castedCurrent->exprList->exprList) {
            result->args.push_back((_expr *) buildSyntaxNode(arg));
        }
        return result;
    }
    else if(syntaxNodeType == "self_dispatch") {
        selfDispatchNode* castedCurrent = (selfDispatchNode*) current;
        _selfDispatch* result = new _selfDispatch{
                castedCurrent->lineNo,
                _idMeta{castedCurrent->IDENTIFIER->lineNo, castedCurrent->IDENTIFIER->value},
        };
        for(exprNode* arg : castedCurrent->exprList->exprList) {
            result->args.push_back((_expr*) buildSyntaxNode(arg));
        }
        return result;
    }
    else if(syntaxNodeType == "if") {
        ifExprNode* castedCurrent = (ifExprNode*) current;
        _if* result = new _if{
            castedCurrent->lineNo,
            (_expr*)buildSyntaxNode(castedCurrent->predicateExpr),
            (_expr*)buildSyntaxNode(castedCurrent->thenExpr),
            (_expr*)buildSyntaxNode(castedCurrent->elseExpr)
        };
        return result;
    }
    else if(syntaxNodeType == "while") {
        whileExprNode* castedCurrent = (whileExprNode*) current;
        _while* result = new _while{
                castedCurrent->lineNo,
                (_expr*)buildSyntaxNode(castedCurrent->predicateExpr),
                (_expr*)buildSyntaxNode(castedCurrent->loopExpr)
        };
        return result;
    }
    else if(syntaxNodeType == "block") {
        blockExprNode* castedCurrent = (blockExprNode*) current;
        _block* result = new _block{castedCurrent->lineNo};
        for(exprNode* expr : castedCurrent->exprList->exprList) {
            result->body.push_back((_expr*)buildSyntaxNode(expr));
        }
        return result;
    }
    else if(syntaxNodeType == "new") {
        newExprNode* castedCurrent = (newExprNode*) current;
        _new* result = new _new{
                castedCurrent->lineNo,
                _idMeta{castedCurrent->TYPE->lineNo, castedCurrent->TYPE->value}
        };
        return result;
    }
    else if(syntaxNodeType == "isvoid") {
        isvoidExprNode* castedCurrent = (isvoidExprNode*)current;
        _isvoid* result = new _isvoid{
            castedCurrent->lineNo,
            (_expr*)buildSyntaxNode(castedCurrent->expr)
        };
        return result;
    }
    else if(syntaxNodeType == "plus" || syntaxNodeType == "minus" || syntaxNodeType == "times" || syntaxNodeType == "divide") {
        arithExprNode* castedCurrent = (arithExprNode*)current;
        _arith* result = new _arith{
            castedCurrent->lineNo,
            (_expr*)buildSyntaxNode(castedCurrent->expr1),
            castedCurrent->productionBody,
            (_expr*)buildSyntaxNode(castedCurrent->expr2),
        };
        return result;
    }
    else if(syntaxNodeType == "lt" || syntaxNodeType == "le" || syntaxNodeType == "eq") {
        relExprNode* castedCurrent = (relExprNode*)current;
        _relational* result = new _relational{
                castedCurrent->lineNo,
                (_expr*)buildSyntaxNode(castedCurrent->expr1),
                castedCurrent->productionBody,
                (_expr*)buildSyntaxNode(castedCurrent->expr2),
        };
        return result;
    }
    else if(syntaxNodeType == "not" || syntaxNodeType == "negate") {
        unaryExprNode* castedCurrent = (unaryExprNode*)current;
        _unary* result = new _unary{
                castedCurrent->lineNo,
                castedCurrent->productionBody,
                (_expr*)buildSyntaxNode(castedCurrent->expr)
        };
        return result;
    }
    else if(syntaxNodeType == "termExpr") {
        termExprNode* castedCurrent = (termExprNode*)current;
        return (_expr*)buildSyntaxNode(castedCurrent->expr);
    }
    else if(syntaxNodeType == "integer") {
        intExprNode* castedCurrent = (intExprNode*)current;
        _integer* result = new _integer{
            castedCurrent->lineNo,
            castedCurrent->INTEGER->value
        };
        return result;
    }
    else if(syntaxNodeType == "string") {
        stringExprNode* castedCurrent = (stringExprNode*)current;
        _string* result = new _string{
                castedCurrent->lineNo,
                castedCurrent->STRING->value
        };
        return result;
    }
    else if(syntaxNodeType == "identifier") {
        identifierExprNode* castedCurrent = (identifierExprNode*)current;
        _identifier* result = new _identifier{
            castedCurrent->lineNo,
            _idMeta{castedCurrent->IDENTIFIER->lineNo, castedCurrent->IDENTIFIER->value}
        };
        return result;
    }
    else if(syntaxNodeType == "true" || syntaxNodeType == "false") {
        boolExprNode* castedCurrent = (boolExprNode*)current;
        _bool* result = new _bool{
            castedCurrent->lineNo,
            castedCurrent->BOOLEAN->value
        };
        return result;
    }
    else if(syntaxNodeType == "let") {
        letExprNode* castedCurrent = (letExprNode*)current;
        _let* result = new _let{
            castedCurrent->lineNo,
            _idMeta{castedCurrent->LET->lineNo, "let" + to_string(_let::letCounter++), "let"},
            (_expr*)buildSyntaxNode(castedCurrent->expr)
        };
        top->links.insert(make_pair(make_pair(result->letKey.identifier, "let"), new Environment{top}));
        top = top->links.at(make_pair(result->letKey.identifier, "let"));
        for(bindingNode* binding : castedCurrent->blNode->bindingList) {
            top->install(make_pair(binding->IDENTIFIER->value, "local"), new Record{binding->IDENTIFIER->value, binding->IDENTIFIER->lineNo, "local"});
            result->bindings.push_back((_letBinding*)buildSyntaxNode(binding));
        }
        top = top->previous;
        return result;
    }
    else if(syntaxNodeType == "let_binding_no_init") {
        bindingNode* castedCurrent = (bindingNode*)current;
        _letBindingNoInit* result = new _letBindingNoInit{
            _idMeta{castedCurrent->IDENTIFIER->lineNo, castedCurrent->IDENTIFIER->value},
            _idMeta{castedCurrent->TYPE->lineNo, castedCurrent->TYPE->value}
        };
        return result;
    }
    else if(syntaxNodeType == "let_binding_init") {
        bindingNode* castedCurrent = (bindingNode*)current;
        _letBindingInit* result = new _letBindingInit{
            _idMeta{castedCurrent->IDENTIFIER->lineNo, castedCurrent->IDENTIFIER->value},
            _idMeta{castedCurrent->TYPE->lineNo, castedCurrent->TYPE->value},
            (_expr*)buildSyntaxNode(castedCurrent->init->expr)
        };
        return result;
    }
    else if(syntaxNodeType == "case") {
        caseExprNode* castedCurrent = (caseExprNode*)current;
        _case* result = new _case {
            castedCurrent->lineNo,
            (_expr*)buildSyntaxNode(castedCurrent->expr)
        };
        for(caseNode* caseElement : castedCurrent->clNode->caseList) {
            //enumerate cases to guarantee a unique key for symbol table
            top->links.insert(make_pair(make_pair("case" + to_string(_caseElement::caseCounter), "case"), new Environment{top}));
            top = top->links.at(make_pair("case" + to_string(_caseElement::caseCounter), "case"));
            result->cases.push_back((_caseElement*)buildSyntaxNode(caseElement));
            top->install(make_pair(caseElement->IDENTIFIER->value, "local"), new Record{caseElement->IDENTIFIER->value, caseElement->IDENTIFIER->lineNo, "local"});
            top = top->previous;
        }
        return result;
    }
    else if(syntaxNodeType == "caseElement") {
        caseNode* castedCurrent = (caseNode*)current;
        _caseElement* result = new _caseElement {
            _idMeta{castedCurrent->IDENTIFIER->lineNo, castedCurrent->IDENTIFIER->value, "local"},
            _idMeta{castedCurrent->TYPE->lineNo, castedCurrent->TYPE->value},
            (_expr*)buildSyntaxNode(castedCurrent->expr),
            _idMeta{castedCurrent->IDENTIFIER->lineNo, "case" + to_string(_caseElement::caseCounter++), "case"}//increment caseCounter here because it's the last time we use it
        };
        return result;
    }


}

/**
 * This is a global map for tokens whose name differ from its lexeme
 */
map<string, string> tokenReqTranslation{
    {"at", "@"},
    {"colon", ":"},
    {"comma", ","},
    {"divide", "/"},
    {"dot", "."},
    {"equals", "="},
    {"larrow", "<-"},
    {"lbrace", "{"},
    {"le", "<="},
    {"lparen", "("},
    {"lt", "<"},
    {"minus", "-"},
    {"plus", "+"},
    {"rarrow", "->"},

    {"rbrace", "}"},
    {"rparen", ")"},
    {"semi", ";"},
    {"tilde", "~"},
    {"times", "*"}
};
