//
// Created by Ivan Rodriguez on 11/5/20.
//

#include "Environment.h"

Record::Record(string lex, int l, string k) :
    lexeme{lex}, lineNo{l}, kind{k}
{

}

Environment::Environment(Environment* prev) :
    previous{prev}
{
    if(previous == nullptr) { //global symtable comes predefined with basic classes
        install(make_pair("Object", "class"), Record{"Object", 0, "class"});
        install(make_pair("Integer", "class"), Record{"Integer", 0, "class"});
        install(make_pair("IO", "class"), Record{"IO", 0, "class"});
        install(make_pair("String", "class"), Record{"String", 0, "class"});
        install(make_pair("Boolean", "class"), Record{"Boolean", 0, "class"});
    }
}

void Environment::install(pair<string, string> key, Record rec) {
    symTable.insert(make_pair(key, rec));
}

Record Environment::get(pair<string, string> key) {
    Environment* current = this;
    while(current != nullptr) {
        map<pair<string, string>, Record>::iterator currentIt = current->symTable.find(key);
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
