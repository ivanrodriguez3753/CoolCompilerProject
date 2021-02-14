//
// Created by Ivan Rodriguez on 2/13/21.
//

#ifndef COOLCOMPILERPROJECT_PARSERDRIVER_H
#define COOLCOMPILERPROJECT_PARSERDRIVER_H

/**
 * This class interfaces with flex and lemon. It carries thread-safe global data, as well as methods implementing
 * procedures associated with a production and methods managing the AST.
 */
class ParserDriver {
public:
    /**
     * Since this class interfaces with flex and lemon, it needs to have trivial constructor.
     */
    ParserDriver() {}

    /**
     * Since this class interfaces with flex and lemon, it needs to have trivial destructor.
     */
    ~ParserDriver() {}


    _program* ast;
    int ctr = 0;




    string id;
    void _program__classList(list<_class*>*& CL) {
        ast = new _program(*CL);
    }
    void classList__classList_class(list<_class*>*& CL1, list<_class*>*& CL2, _class**& C) {
        CL1 = CL2;
        (*CL1).push_back(*C);
    }
    void classList(list<_class*>*&  CL) {
        CL = new list<_class*>;
    }
    void class__CLASS_TYPE_LBRACE_RBRACE_SEMI(_class**& C) {
        C = new _class*;
        *C = new _class(id, "Object");
    }
};


#endif //COOLCOMPILERPROJECT_PARSERDRIVER_H
