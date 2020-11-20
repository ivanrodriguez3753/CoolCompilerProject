//
// Created by Ivan Rodriguez on 11/5/20.
//

#include "Environment.h"

int Record::orderCounter = 0;
Record::Record(string lex, int l, string k) :
    lexeme{lex}, lineNo{l}, kind{k}, encountered{orderCounter++}
{

}

Environment::Environment(Environment* prev) :
    previous{prev}
{
    if(previous == nullptr) { //global symtable comes predefined with basic classes
        install(make_pair("Object", "class"), new classRecord{"Object", 0, "class", ""});
        install(make_pair("Int", "class"), new classRecord{"Int", 0, "class", "Object"});
        install(make_pair("IO", "class"), new classRecord{"IO", 0, "class", "Object"});
        install(make_pair("String", "class"), new classRecord{"String", 0, "class", "Object"});
        install(make_pair("Bool", "class"), new classRecord{"Bool", 0, "class", "Object"});
    }
}

void Environment::install(pair<string, string> key, Record* rec) {
    symTable.insert(make_pair(key, rec));
}

Record* Environment::get(pair<string, string> key) {
    Environment* current = this;
    while(current != nullptr) {
        map<pair<string, string>, Record*>::iterator currentIt = current->symTable.find(key);
        if(currentIt != current->symTable.end()) { //found key
            return currentIt->second;
        }
        current = current->previous; //didn't find key, go to surrounding scope and search again
    }
}

void Environment::reset() {
    globalEnv = new Environment{nullptr};
    top = globalEnv;
}

Environment* globalEnv = new Environment{nullptr};
Environment* top = globalEnv;

classRecord::classRecord(string lex, int l, string k, string p) :
    Record{lex, l, k}, parent{p}
{

}

objectRecord::objectRecord(string lex, int l, string k, string t, _expr* init) :
    Record{lex, l, k}, type{t}, initExpr{init}
{

}
