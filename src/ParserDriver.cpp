#include "ParserDriver.hh"
#include "parser.hpp"
#include "environment.h"
#include <set>
#include <stack>

#include "llvm/ADT/STLExtras.h"

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

/**
 * hard code the internalsAst
 */
void ParserDriver::buildInternalsAst() {
    internalsAst =
    new _program(0, vector<_class*>{
        new _class(0, 0, "Object", "Object", pair<vector<_attr*>, vector<_method*>>{
            vector<_attr*>(),
            vector<_method*>{
                new _method(0, 0, "abort", "Object", {}, new _internal("Object.abort"), encountered++),
                new _method(0, 0, "copy", "SELF_TYPE", {}, new _internal("Object.copy"), encountered++),
                new _method(0, 0, "type_name", "String", {}, new _internal("Object.type_name"), encountered++)
            }
        }),
        new _class(0, 0, "IO", "Object", pair<vector<_attr*>, vector<_method*>>{
                vector<_attr*>(),
                vector<_method*>{
                        new _method(0, 0, "in_int", "Int", {}, new _internal("IO.in_int"), encountered++),
                        new _method(0, 0, "in_string", "String", {}, new _internal("IO.in_string"), encountered++),
                        new _method(0, 0, "out_int", "SELF_TYPE", {new _formal(0, 0, "x", "Int")}, new _internal("IO.out_int"), encountered++),
                        new _method(0, 0, "out_string", "SELF_TYPE", {new _formal(0,0, "x", "String")}, new _internal("IO.out_string"), encountered++)
                }
        }),
        new _class(0, 0, "String", "Object", pair<vector<_attr*>, vector<_method*>>{
                vector<_attr*>(),
                vector<_method*>{
                        new _method(0, 0, "concat", "String", {new _formal(0, 0, "s", "String")}, new _internal("String.concat"), encountered++),
                        new _method(0, 0, "length", "Int", {}, new _internal("String.length"), encountered++),
                        new _method(0, 0, "substr", "String", {new _formal(0, 0, "i", "Int"), new _formal(0, 0, "l", "Int")}, new _internal("String.substr"), encountered++)
                }
        }),
        new _class(0, 0, "Int", "Object", {}),
        new _class(0, 0, "Bool", "Object", {})
    });
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

            class_env->symTable.insert({attr->id, new objRec(attr, attr->lineNo, i, attr->type)});
        }
        vector<_method*>& methods = klass->featureList.second;
        for(int i = 0; i < methods.size(); ++i) {
            _method* method = methods[i];

            methodEnv* method_env = new methodEnv(class_env, method->id);

            class_env->methodsSymTable.insert({method->id,
                new methodRec(method, method->lineNo, method->formalsList.size(), method_env, i, method->returnType)});
        }
        int formalCtr = 0;
        for(auto method : class_env->methodsSymTable) {
            method.second->link->symTable.insert({"self", new objRec(nullptr, 0, -1, "SELF_TYPE")}); //TODO: set offset to a negative but investigate what needs to be done so that self is in the symTable for each method but doesn't affect other things
            for(auto formal : ((_method*)(method.second->treeNode))->formalsList) {
                method.second->link->symTable.insert({formal->id, new objRec(formal, formal->lineNo, formalCtr++, formal->type)});
            }
        }
    }
}

/**
 * helper method that populates the entry in class/implementation maps corresponding to klass
 * @param klass
 */
void ParserDriver::populateMaps(string klass) {
    set<string> attrsAlreadyAdded;

    map<string, pair<objRec*, int>>& attrs = classMap[klass];
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
        vector<pair<string, objRec*>> orderedAttrs;
        for(pair<string, rec*> attrPair : curClassEnv->symTable) {
            orderedAttrs.push_back({attrPair.first, (objRec*)attrPair.second});
        }
        sort(orderedAttrs.begin(), orderedAttrs.end(), [](const pair<string, objRec*> lhs, const pair<string, objRec*> rhs) {
            return lhs.second->localOffset < rhs.second->localOffset;
        });
        for(pair<string, objRec*> attr : orderedAttrs) {
           if(attrsAlreadyAdded.find(attr.first) != attrsAlreadyAdded.end()) {
               cerr << "this attr has already been defined\n";
               cerr << "skipping redefinition of this attr\n";
               continue;
            }
            attrs.insert({attr.first, {curClassEnv->getRec(attr.first), attrCounter++}});
        }

        //methods
        //map has them in ascending alphabetical order, but we want to iterate in the order we encountered
        vector<pair<string, methodRec*>> orderedMethods(curClassEnv->methodsSymTable.begin(), curClassEnv->methodsSymTable.end());
        sort(orderedMethods.begin(), orderedMethods.end(), [](const pair<string, methodRec*> lhs, const pair<string, methodRec*> rhs) {
            return lhs.second->encountered < rhs.second->encountered;
        });


        for(vector<pair<string, methodRec*>>::iterator it = orderedMethods.begin(); it != orderedMethods.end(); it++) {
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
    map<string, bool> visited;


    vector<_class*> classListFull = ast->classList;
    classListFull.insert(classListFull.end(), internalsAst->classList.begin(), internalsAst->classList.end());

    //initialize
    for(_class* klass : classListFull) {
        inherGraph.insert({klass->id, {klass->superId, set<string>()}});
    }
    //populate
    for(_class* klass : classListFull) {
        inherGraph[klass->superId].second.insert(klass->id);
        visited[klass->id] = false;
    } visited["Object"] = false;

    //BEGIN DEPTH FIRST TRAVERSAL
    stack<string> S;
    S.push("Object");
    while(!S.empty()) {
        string v = S.top(); S.pop();
        bool& isVisited = visited.at(v);
        set<string>& children = inherGraph.at(v).second;
        if(!isVisited) {
            isVisited = true;
            populateMaps(v);
            for(string child : children) {
                S.push(child);
            }
        }
    }

}

void ParserDriver::printClassMap(ostream& os){
    os << "class_map" << endl;
    os << classMap.size() << endl;

    //C++ maps are ordered by std::less by default, and we need it to print classes in alphabetical order
    for(map<string, map<string, pair<objRec*, int>>>::iterator it = classMap.begin(); it != classMap.end(); it++) {
        os << it->first << endl;
        os << it->second.size() << endl;

        map<string, pair<objRec*, int>> currentMap = it->second;

//        essentially convert from map w location info to a vector
        vector<pair<objRec*, int>> orderedAttributes;
        for(pair<string, pair<objRec*, int>> attr : currentMap) {
            orderedAttributes.push_back(attr.second);
        }
        sort(orderedAttributes.begin(), orderedAttributes.end(), [](const pair<objRec*, int> lhs, const pair<objRec*, int> rhs) {
            return lhs.second < rhs.second;
        });
        //print them, now that they are in order
        for(pair<objRec*, int> objPair : orderedAttributes) {
            objRec* obj = objPair.first;
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

/**
 * This method is compatible with and will return SELF_TYPE if appropriate, in unresolved form (the literal "SELF_TYPE")
 * Compute the least upper bound of the classes in the set, that is, return the most recent common ancestor
 * @param s
 * @return
 */
string ParserDriver::computeLub(set<string> s) {
    if(!s.size()) {
        cerr << "should not have been called with an empty set\n";
        abort();
    }
    else if(s.size() == 1) return *(s.begin());

    //resolve SELF_TYPE choice, if any
    bool removedSELF_TYPE = false;
    set<string>::iterator selfTypeIt = s.find("SELF_TYPE");
    if(selfTypeIt != s.end()) {
        removedSELF_TYPE = true;
        s.erase(selfTypeIt);

        //resolve SELF_TYPE for the comparison/tree traversal
        s.insert(currentClassEnv->id);
    }

    map<string, vector<string>> inheritancePaths;
    for(string klass : s) {
        vector<string>& currentPath = inheritancePaths[klass];
        string currentParent = inherGraph.at(klass).first;
        while(currentParent != "Object") {
            currentPath.push_back(currentParent);
            currentParent = inherGraph.at(currentParent).first;

        }
    }

    //reverse all paths because we read them bottom up, also note the shortestPath
    int shortestPath = inheritancePaths.begin()->second.size();
    for(map<string, vector<string>>::iterator it = inheritancePaths.begin(); it != inheritancePaths.end(); it++) {
        reverse(it->second.begin(), it->second.end());
        if(it->second.size() < shortestPath) {
            shortestPath = it->second.size();
        }
    }

    string lub;

    if(shortestPath == 0) {
        lub =  "Object";
    }
    else {
        //get an arbitrary path and return the node that is shortestPath - 1 steps down from Object
        const vector<string>& somePath = (*(inheritancePaths.begin())).second;
        lub = somePath[shortestPath];
    }

    if(removedSELF_TYPE && (lub == currentClassEnv->id)) lub = "SELF_TYPE"; //if the answer ended up being the resolved SELF_TYPE, we need to unresolve

    return lub;

}

letCaseEnv* ParserDriver::buildLetEnv(_let* letNode) {
    letCaseEnv* returnThis = new letCaseEnv(top, "let" + to_string(encountered));

    for(int i = 0; i < letNode->bindingList.size(); ++i) {
        returnThis->symTable.insert({letNode->bindingList[i]->id, new objRec(letNode->bindingList[i], letNode->bindingList[i]->lineNo, i, letNode->bindingList[i]->type)});
    }
    return returnThis;
}

vector<letCaseEnv*> ParserDriver::buildCaseEnvs(_case* caseNode) {
    vector<letCaseEnv*> returnThis;
    for(int i = 0; i < caseNode->caseList.size(); ++i) {
        returnThis.push_back(
            new letCaseEnv(top, "case" + to_string(encountered) + '_' + to_string(i)));
        _caseElement* caseElem = caseNode->caseList[i];
        returnThis[i]->symTable.insert({caseElem->id, new objRec(caseElem, caseElem->lineNo, i, caseElem->type)});
    }
    return returnThis;

}

/**
 * the caller, _class::decorate(ParserDriver& drv), has set up the drv.currentMethodEnv context, whose symTable has
 * all attributes (including inherited attributes) and a self entry
 * @param drv
 */
void _class:: decorateAttrInitExprs(ParserDriver &drv) {
    for(_attr* attr : featureList.first) {
        if(attr->optInit) attr->optInit->decorate(drv);
    }
}
