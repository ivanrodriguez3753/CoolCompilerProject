#include "ParserDriver.hh"
#include "parser.hpp"
#include "environment.h"
#include <set>
#include <stack>

ParserDriver::ParserDriver() : trace_parsing{false}, trace_scanning{false} {}

int ParserDriver::parse(const std::string& f) {
    file = f;
    location.initialize(&file);
    scan_begin();
    yy::parser parse(*this);
    parse.set_debug_level(trace_parsing);
    int res = parse();
    scan_end();

    ast = bisonProduct;

    return res;
}
int ParserDriver::parseBasicClasses(const string &source) {
    location.initialize();
    scan_string();
    yy::parser parse(*this);
    int res = parse();
    scan_end();

    return res;
}

void ParserDriver::buildInternalsAst() {
    parseBasicClasses(basicClassesSource);

    internalsAst = bisonProduct;
}





void ParserDriver::buildEnvs() {
    vector<_class*> classListFull = ast->classList;
    classListFull.insert(classListFull.end(), internalsAst->classList.begin(), internalsAst->classList.end());

    env = new globalEnv(nullptr, "global"); //TODO: ADD THREAD NUMBER


    for(_class* klass : classListFull) {
        classEnv* class_env = new classEnv(env, klass->id);
        if(env->symTable.find(klass->id) != env->symTable.end()) {
            cerr << "this class has already been defined\n";
            cerr << "skipping redefinition of this class\n";
            continue;
        }
        env->symTable.insert({klass->id,
            new classRec(klass, klass->lineNo, klass->superId, klass->featureList.first.size(), klass->featureList.second.size(), class_env)});

        vector<_attr*>& attrs = klass->featureList.first;
        for(int i = 0; i < attrs.size(); ++i) {
            _attr* attr = attrs[i];

            class_env->symTable.insert({attr->id, new objRec(attr, attr->lineNo, i)});
        }
        vector<_method*>& methods = klass->featureList.second;
        for(int i = 0; i < methods.size(); ++i) {
            _method* method = methods[i];

            methodEnv* method_env = new methodEnv(class_env, method->id);

            class_env->methodsSymTable.insert({method->id,
                new methodRec(method, method->lineNo, method->formalsList.size(), method_env)});
        }
    }
}

/**
 * helper method that populates the entry in class/implementation maps corresponding to klass
 * @param graph
 * @param klass
 */
void ParserDriver::populateMaps(map<string, set<string>>& graph, string klass) {
    set<string> attrsAlreadyAdded;

    set<pair<objRec*, int>>& attrs = classMap[klass];
    map<string, pair<methodRec*, int>>& methods = implementationMap[klass];

    int attrCounter = 0;
    int methodCounter = 0;

    vector<string> bottomUpOrder;
    while(klass != "Object") {
        bottomUpOrder.push_back(klass);
        klass = env->getRec(klass)->parent;
    } bottomUpOrder.push_back(klass);

    //insert all attributes and methods starting from Object
    for(int i = bottomUpOrder.size() - 1; i >= 0; --i) {
        string& currentClass = bottomUpOrder[i];
        classEnv* curClassEnv = env->getRec(currentClass)->link;

        //attributes
        for(map<string, rec*>::iterator it = curClassEnv->symTable.begin(); it != curClassEnv->symTable.end(); it++) {
           if(attrsAlreadyAdded.find(it->first) != attrsAlreadyAdded.end()) {
               cerr << "this attr has already been defined\n";
               cerr << "skipping redefinition of this attr\n";
               continue;
            }
            attrs.insert({curClassEnv->getRec(it->first), attrCounter++});
        }

        //methods
        for(map<string, methodRec*>::iterator it = curClassEnv->methodsSymTable.begin(); it != curClassEnv->methodsSymTable.end(); it++) {
            if(methods.find(it->first) == methods.end()) { //did not find this method in the map
                methods.insert({it->first, {it->second, methodCounter++}});
            }
            else {
                methods.at(it->first).first = it->second;
            }
        }
    }

}

void ParserDriver::populateClassImplementationMaps() {
    //build a quick graph. first is node, second is node's data which children
    map<string, set<string>> graph;
    map<string, bool> visited;


    vector<_class*> classListFull = ast->classList;
    classListFull.insert(classListFull.end(), internalsAst->classList.begin(), internalsAst->classList.end());

    for(_class* klass : classListFull) {
        graph.insert({klass->id, set<string>()});
        //note that [] creates an entry if it doesn't exist
        graph[klass->superId].insert(klass->id);
        visited[klass->id] = false;
    } visited["Object"] = false;

    //BEGIN DEPTH FIRST TRAVERSAL
    stack<string> S;
    S.push("Object");
    while(!S.empty()) {
        string v = S.top(); S.pop();
        bool& isVisited = visited.at(v);
        set<string>& children = graph.at(v);
        if(!isVisited) {
            isVisited = true;
            populateMaps(graph, v);
            for(string child : children) {
                S.push(child);
            }
        }
    }

}

void ParserDriver::printClassMap(ostream& os){
    os << "class_map" << endl;
    os << classMap.size() << endl;

    //C++ maps are ordered by std::less by default, and we need it to print in alphabetical order
    for(map<string, set<pair<objRec*, int>>>::iterator it = classMap.begin(); it != classMap.end(); it++) {
        os << it->first << endl;
        os << it->second.size() << endl;




        //essentially convert from map w location info to a vector
        vector<objRec*> orderedAttributes(it->second.size());
        for(pair<objRec*, int> attr : it->second) {
            orderedAttributes[attr.second] = attr.first;
        }

        //print them, now that they are in order
        for(objRec* obj : orderedAttributes) {
            _attr* attrObj = (_attr*)obj->treeNode;
            if(attrObj->optInit) {
                os << "initializer" << endl;
                os << attrObj->id << endl;
                os << attrObj->type << endl;
                attrObj->optInit->print(os);
            }
            else {
                os << "no_initializer" << endl;
                os << attrObj->id << endl;
                os << attrObj->type << endl;
            }
        }
    }
}
void ParserDriver::printImplementationMap(ostream& os) {
    os << "implementation_map" << endl;
    os << implementationMap.size() << endl;

    for(auto classIt : implementationMap) {
        os << classIt.first << endl;
        os << classIt.second.size() << endl;


        vector<methodRec*> orderedMethods(classIt.second.size());
        for(auto methodIt : classIt.second) {
            orderedMethods[methodIt.second.second] = methodIt.second.first;
        }

        for(methodRec* methodRec : orderedMethods) {
            _method* method = (_method*)methodRec->treeNode;

            os << method->id << endl;
            os << method->formalsList.size() << endl;

            for(_formal* formal : method->formalsList) {
                os << formal->id << endl;
            }

            //output most recent definer
            os << (methodRec->link->prev)->id << endl;

            //print the body expression
            ((_method*)methodRec->treeNode)->body->print(os);
        }

    }
}
void ParserDriver::printParentMap(ostream& os) {
    map<string,rec*> symTableCopy = env->symTable;
    symTableCopy.erase("Object");

    os << "parent_map" << endl;
    os << symTableCopy.size()<< endl;
    //default ordering is sfine
    for(map<string, rec*>::iterator classIt = symTableCopy.begin(); classIt != symTableCopy.end(); classIt++) {
        os << classIt->first << endl;
        os << ((classRec*)(classIt->second))->parent << endl;
    }
}