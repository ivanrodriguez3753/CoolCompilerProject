//
// Created by Ivan Rodriguez on 11/5/20.
//

#include "Environment.h"
#include "type.h"

Environment* globalEnv = new Environment{nullptr, Environment::envMetaInfo{"", ""}};
Environment* top = globalEnv;




Environment::Environment(Environment* prev, envMetaInfo info) :
    previous{prev}, metaInfo{info}
{
    /**
     * global environment constructor.
     * everything on this symbol table should be a class
     */
    if(previous == nullptr) { //global symtable comes predefined with basic classes, and some of those have predefined/system calls
        //global is represented by the root node, which is a _program type. That doesn't have an _idMeta, so we'll make the identifier "global" and the kind "global"
        metaInfo.identifier = metaInfo.kind = "global";
        metaInfo.depth = 0;
        //NOTE: methods are generally printed in the order they are encountered in the code. Internal methods, however,
        //are defined to be "encountered" in the same order as if they were sorted alphabetically. So the list order
        //will be different then the block comment order, so that the records are constructed with the approriate "encountered" counter value
        //==============OBJECT====================
        //Every method in Object has no parameters
        list<string> lexemes{"abort", "copy", "type_name"};
        list<list<pair<string, string>>> parameters{{},{},{}};
        list<string> returnTypes{"Object", "SELF_TYPE", "String"};
       /** abort() : Object
         * type_name() : String
         * copy() : SELF_TYPE   */
        install(make_pair("Object", "class"), new classRecord{"Object", 0, "class", ""});
        links.insert(make_pair(make_pair("Object", "class"), new Environment{this,envMetaInfo{"Object", "class"} }));
        methodRecord::makeAndInstallMethodsRecordAndEnv(lexemes, parameters, returnTypes, links.at(make_pair("Object", "class")));

        //==============IO===================
        lexemes = list<string>{"in_int", "in_string", "out_int", "out_string"};
        parameters = list<list<pair<string, string>>>{list<pair<string, string>>{}, list<pair<string, string>>{}, list<pair<string, string>>{make_pair("x", "String")},list<pair<string, string>>{make_pair("x", "String")}};
        returnTypes = list<string>{"Int", "String", "SELF_TYPE", "SELF_TYPE"};
        /** * out_string(x : String) : SELF_TYPE
            * out_int(x : Int) : SELF_TYPE
            * in_string() : String
            * in_int() : Int                  */
        install(make_pair("IO", "class"), new classRecord{"IO", 0, "class", "Object"});
        links.insert(make_pair(make_pair("IO", "class"), new Environment{this, envMetaInfo{"IO", "class"}}));
        methodRecord::makeAndInstallMethodsRecordAndEnv(lexemes, parameters, returnTypes, links.at(make_pair("IO", "class")));

        //=============STRING===================
        lexemes = list<string>{"concat", "length", "substr"};
        parameters = list<list<pair<string, string>>>{list<pair<string, string>>{make_pair("s", "String")}, list<pair<string, string>>{}, list<pair<string, string>>{make_pair("i", "Int"), make_pair("l", "Int")}};
        returnTypes = list<string>{"String", "Int", "String"};
        /**length() : Int
         * concat(s : String) : String
         * substr(i : Int, l : Int) : String*/
        install(make_pair("String", "class"), new classRecord{"String", 0, "class", "Object"});
        links.insert(make_pair(make_pair("String", "class"), new Environment{this, envMetaInfo{"String", "class"}}));
        methodRecord::makeAndInstallMethodsRecordAndEnv(lexemes, parameters, returnTypes, links.at(make_pair("String", "class")));

        //==============INT====================
        //Int has no methods (except those from Object)
        install(make_pair("Int", "class"), new classRecord{"Int", 0, "class", "Object"});
        links.insert(make_pair(make_pair("Int", "class"), new Environment{this, envMetaInfo{"Int", "class"}}));


        //==============BOOL====================
        //Bool has no methods (except those from Object)
        install(make_pair("Bool", "class"), new classRecord{"Bool", 0, "class", "Object"});
        links.insert(make_pair(make_pair("Bool", "class"), new Environment{this, envMetaInfo{"Bool", "class"}}));


        return;
    }
    if(metaInfo.kind == "class") {
        install({"self", "attribute"}, new objectRecord{this, "self", 0, "attribute", "SELF_TYPE", nullptr}); //todo fix this nullptr
    }
    metaInfo.depth = previous->metaInfo.depth + 1;
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
    return nullptr;
}

void Environment::reset() {
    globalEnv = new Environment{nullptr, Environment::envMetaInfo{"",""}};
    top = globalEnv;
}

vector<methodRecord *> Environment::getMethods() {
    vector<methodRecord*> returnThis;
    for(auto sym : symTable) {
        if(sym.first.second == "method") {
            returnThis.push_back((methodRecord*)sym.second);
        }
    }
    return returnThis;
}



int Record::orderCounter = 0;
Record::Record(Environment* container, string lex, int l, string k) :
        containerEnv{container}, lexeme{lex}, lineNo{l}, kind{k}, encountered{orderCounter++}
{

}

classRecord::classRecord(string lex, int l, string k, string p) :
    Record{globalEnv, lex, l, k}, parent{p}
{

}

objectRecord::objectRecord(Environment* container, string lex, int l, string k, string t, _expr* init) :
    Record{container, lex, l, k}, type{t}, initExpr{init}
{

}

//parallel lists of method identifiers, formal parameter list (identifier, type) and return types
void methodRecord::makeAndInstallMethodsRecordAndEnv(list<string> lexemes, list<list<pair<string, string>>> parameters, list<string>returnTypes, Environment *classEnv) {
    while(!lexemes.empty()) {
        classEnv->install(make_pair(lexemes.front(), "method"),
                          new methodRecord{classEnv,lexemes.front(), 0, "method", returnTypes.front()});
        classEnv->links.emplace(make_pair(lexemes.front(), "method"), new Environment{classEnv, Environment::envMetaInfo{lexemes.front(), "method"}});
        Environment *methodEnvironment = (classEnv->links.at(make_pair(lexemes.front(), "method")));
        for (list<pair<string, string>> parameter : parameters) {
            methodEnvironment->install(make_pair(parameter.front().first, "local"),
                                       new objectRecord{methodEnvironment, parameter.front().first, 0, "local", parameter.front().second, nullptr});
        }
        lexemes.pop_front(); parameters.pop_front(); returnTypes.pop_front();
    }
}

methodRecord::methodRecord(Environment* container, string lex, int l, string k, string rt) :
    Record{container, lex, l, k}, returnType{rt}
{

}
