#include "type.h"
#include "Environment.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include "syntaxTreeNodes.h"


map<string, classRecord*> classMap{};
/**
 * Parent map is kinda redundant, it's just a shallow copy without "Object"
 */
map<string, classRecord*> parentMap{};

/**
 * print error to stderr and push to log
 * @param error
 */
void printAndPush(pair<int,string> error) {
    cerr << "ERROR: " << to_string(error.first) << ", " << error.second << endl;
    errorLog.push_back(error);
}

void buildBasicClassNodes() {
    //Use to conveniently access scope I need
    map<pair<string, string>, Record*> currentSymTable = globalEnv->links.at(make_pair("Object", "class"))->symTable;
    //TODO I did not link the _formal nodes to it's corresponding objectRecord

    //don't want basic classes on the AST, but I do want _class nodes for them. The symTable is not separated like the user AST and Basic classes AST are, so every
    //record in the symtable has a corresponding AST node even if there are two ASTs
    Object_class = new _classNoInh{_idMeta(0, "Object", "class"), (classRecord*)globalEnv->symTable.at(make_pair("Object", "class"))};
    ((classRecord*)globalEnv->symTable.at(make_pair("Object", "class")))->treeNode = Object_class;
    Object_class->featureList.push_back(new _method{_idMeta{0, "abort", "method"}, _idMeta{0, "Object", "class"}, new _internal{"Object", "abort"}});
    ((methodRecord*)currentSymTable.at(make_pair("abort", "method")))->treeNode = (_method*)Object_class->featureList.back();
    ((_method*)Object_class->featureList.back())->body->exprType = "Object";
    Object_class->featureList.push_back(new _method{_idMeta{0, "copy", "method"}, _idMeta{0, "SELF_TYPE", "class"}, new _internal{"Object", "copy"}});
    ((methodRecord*)currentSymTable.at(make_pair("copy", "method")))->treeNode = (_method*)Object_class->featureList.back();
    ((_method*)Object_class->featureList.back())->body->exprType = "SELF_TYPE";
    Object_class->featureList.push_back(new _method{_idMeta{0, "type_name", "method"}, _idMeta{0, "String", "class"}, new _internal{"Object", "type_name"}});
    ((methodRecord*)currentSymTable.at(make_pair("type_name", "method")))->treeNode = (_method*)Object_class->featureList.back();
    ((_method*)Object_class->featureList.back())->body->exprType = "String";


    currentSymTable = globalEnv->links.at(make_pair("IO", "class"))->symTable;
    IO_class = new _classInh{_idMeta(0, "IO", "class"), (classRecord*)globalEnv->symTable.at(make_pair("IO", "class")), _idMeta{0, "Object", "class"}};
    ((classRecord*)globalEnv->symTable.at(make_pair("IO", "class")))->treeNode = IO_class;
    IO_class->featureList.push_back(new _method{_idMeta{0, "in_int", "method"}, _idMeta{0, "Int", "class"}, new _internal{"IO", "in_int"}});
    ((methodRecord*)currentSymTable.at(make_pair("in_int", "method")))->treeNode = (_method*)IO_class->featureList.back();
    ((_method*)IO_class->featureList.back())->body->exprType = "Int";
    IO_class->featureList.push_back(new _method{_idMeta{0, "in_string", "method"}, _idMeta{0, "String", "class"}, new _internal{"IO", "in_string"}});
    ((methodRecord*)currentSymTable.at(make_pair("in_string", "method")))->treeNode = (_method*)IO_class->featureList.back();
    ((_method*)IO_class->featureList.back())->body->exprType = "String";
    IO_class->featureList.push_back(new _method{_idMeta{0, "out_int", "method"}, _idMeta{0, "SELF_TYPE", "class"}, new _internal{"IO", "out_int"}});
    ((methodRecord*)currentSymTable.at(make_pair("out_int", "method")))->treeNode = (_method*)IO_class->featureList.back();
    ((_method*)IO_class->featureList.back())->body->exprType = "SELF_TYPE";
    ((_method*)(IO_class->featureList.back()))->formalList.push_back(new _formal{_idMeta{0, "x"}, _idMeta{0, "Int"}});
    IO_class->featureList.push_back(new _method{_idMeta{0, "out_string", "method"}, _idMeta{0, "SELF_TY[E", "class"}, new _internal{"IO", "out_string"}});
    ((methodRecord*)currentSymTable.at(make_pair("out_string", "method")))->treeNode = (_method*)IO_class->featureList.back();
    ((_method*)IO_class->featureList.back())->body->exprType = "SELF_TYPE";
    ((_method*)(IO_class->featureList.back()))->formalList.push_back(new _formal{_idMeta{0, "x"}, _idMeta{0, "String"}});

    Bool_class = new _classInh{_idMeta(0, "Bool", "class"), (classRecord*)globalEnv->symTable.at(make_pair("Bool", "class")), _idMeta{0, "Object", "class"}};
    ((classRecord*)globalEnv->symTable.at(make_pair("Bool", "class")))->treeNode = Bool_class;

    currentSymTable = globalEnv->links.at(make_pair("String", "class"))->symTable;
    String_class = new _classInh{_idMeta(0, "String", "class"), (classRecord*)globalEnv->symTable.at(make_pair("String", "class")), _idMeta{0, "Object", "class"}};
    ((classRecord*)globalEnv->symTable.at(make_pair("String", "class")))->treeNode = String_class;
    String_class->featureList.push_back(new _method{_idMeta{0, "concat", "method"}, _idMeta{0, "String", "class"}, new _internal{"String", "concat"}});
    ((methodRecord*)currentSymTable.at(make_pair("concat", "method")))->treeNode = (_method*)String_class->featureList.back();
    ((_method*)String_class->featureList.back())->body->exprType = "String";
    ((_method*)(String_class->featureList.back()))->formalList.push_back(new _formal{_idMeta{0, "s"}, _idMeta{0, "String"}});
    String_class->featureList.push_back(new _method{_idMeta{0, "length", "method"}, _idMeta{0, "Int", "class"}, new _internal{"String", "length"}});
    ((methodRecord*)currentSymTable.at(make_pair("length", "method")))->treeNode = (_method*)String_class->featureList.back();
    ((_method*)String_class->featureList.back())->body->exprType = "Int";
    String_class->featureList.push_back(new _method{_idMeta{0, "substr", "method"}, _idMeta{0, "String", "class"}, new _internal{"String", "substr"}});
    ((methodRecord*)currentSymTable.at(make_pair("substr", "method")))->treeNode = (_method*)String_class->featureList.back();
    ((_method*)String_class->featureList.back())->body->exprType = "String";
    ((_method*)(String_class->featureList.back()))->formalList.push_back(new _formal{_idMeta{0, "i"}, _idMeta{0, "Int"}});
    ((_method*)(String_class->featureList.back()))->formalList.push_back(new _formal{_idMeta{0, "l"}, _idMeta{0, "Int"}});

    Int_class = new _classInh{_idMeta(0, "Int", "class"), (classRecord*)globalEnv->symTable.at(make_pair("Int", "class")), _idMeta{0, "Object", "class"}};
    ((classRecord*)globalEnv->symTable.at(make_pair("Int", "class")))->treeNode = Int_class;
}

/**
 * Return a vector containing the class hierarchy starting at klass
 * @param klass
 * @return
 */
vector<string> getInheritancePath(string klass) {
    vector<string> inheritancePath{};

    if(klass == "SELF_TYPE") {
        return vector<string>{"SELF_TYPE", "Object"};
    }
    classRecord* currentRec = classMap.at(klass);
    while(currentRec->parent != "") { //Object's parent is the empty string
        inheritancePath.push_back(currentRec->lexeme);
        currentRec = (classRecord*)globalEnv->symTable.at(make_pair(currentRec->parent, "class"));
    }
    inheritancePath.push_back(currentRec->lexeme);
    return inheritancePath;
}

void populateClassMap() {
    for(map<pair<string, string>, Record*>::iterator entryIterator = globalEnv->symTable.begin(); entryIterator != globalEnv->symTable.end(); entryIterator++) {
        //entryIterator.first.first is the lexeme (entryIterator.first.second is the kind)
        //entryIterator.second is a Record, so pass its address
        string first = entryIterator->first.first;
        Record* second = (entryIterator->second);
        classMap.insert(make_pair(first, (classRecord*)second));
    }
}
void printClassMap(ostream& out) {
    vector<classRecord*> recordRefs;
    for(auto entryIterator : classMap) {
        recordRefs.push_back(((classRecord*)entryIterator.second));
    }
    //sort by ascending alphabetical order
    sort(recordRefs.begin(), recordRefs.end(), [](const classRecord* lhs, const classRecord* rhs) {
        return lhs->lexeme < rhs->lexeme;
    });

    out << "class_map\n" << recordRefs.size() << endl;
    for(auto rec : recordRefs) {
        top = top->links.at({rec->lexeme,"class"});


        string klass = rec->lexeme;
        out << klass << endl;
        list<vector<pair<objectRecord*, string>>> attributesByClassInHierarchy{};
        vector<pair<objectRecord*, string>> attributes; //objectRecord and definingClass
        vector<string> hierarchyTraversalKeys; //need info about path we took to get back
        if(klass != "Object" && klass != "Int" && klass != "IO" && klass != "Bool" && klass != "String") {
            //top = top->links.at(make_pair(rec->lexeme, "class"));
            //we need every attribute for each class held by rec : recordRefs, so we first have to climb the the inheritance tree
            //up to but not including Object (has no attributes to print anyway, and is the root of the hierarchy)
            classRecord* currentClassRec = rec;
            classRecord* currentParentRec = classMap.at(rec->parent);
            hierarchyTraversalKeys.push_back(currentClassRec->lexeme);
            while(currentClassRec->lexeme != "Object") {
                currentParentRec = currentClassRec;
                if(classMap.find(currentClassRec->parent) != classMap.end()) {
                    currentClassRec = classMap.at(currentClassRec->parent);
                    hierarchyTraversalKeys.push_back(currentClassRec->lexeme);
                }
            }
            //traverse the hierarchy (starting one before Object) back to where we started, pushing back attributes as we encounter them
            for(int i = hierarchyTraversalKeys.size() - 2; i >= 0; --i) {
                currentClassRec = classMap.at(hierarchyTraversalKeys[i]);
                for(auto klasssIt : globalEnv->links.at(make_pair(currentClassRec->lexeme, "class"))->symTable) { //get all attributes for this class
                    if(klasssIt.second->kind == "attribute") {
                        attributes.push_back({(objectRecord*)klasssIt.second, currentClassRec->lexeme});
                    }
                }
                sort(attributes.begin(), attributes.end(), [](const pair<objectRecord*, string> lhs, const pair<objectRecord*, string> rhs) { //sort attributes in this class by encountered counter
                    return lhs.first->encountered < rhs.first->encountered;
                });
                attributesByClassInHierarchy.push_back(attributes);
                attributes.clear();

            }


        }

        //flatten the list of vectors for the attributes, can reuse the intermediary vector
        for(auto currentList : attributesByClassInHierarchy) {
            for(auto currentRec : currentList) {
                attributes.push_back(currentRec);
            }
        }

        //remove all selfs because we don't want to print them as attributes
        vector<pair<objectRecord*, string>> attributes2;
        for(auto attribute : attributes) {
            if(attribute.first->lexeme != "self") {
                attributes2.push_back(attribute);
            }
        }
        attributes = attributes2;
        out << attributes.size() << endl;

        for(auto attribute : attributes) {
            if(attribute.first->lexeme == "self") continue; //i don't think self needs to be printed, or else it'd be printed once per class in hierarchy
            //make sure we're on the right symbol table for each attribute
            top = globalEnv->links.at({attribute.second ,"class"});
            if(attribute.first->initExpr) {
                out << "initializer\n";
                out << attribute.first->lexeme << endl << attribute.first->type << endl;
                out << *(attribute.first->initExpr);
            }
            else {
                out << "no_initializer\n";
                out << attribute.first->lexeme << endl << attribute.first->type << endl;
            }

        }

        top = top->previous;
    }

}
void fillInClassAttributeNum() {
    vector<classRecord*> recordRefs;
    for(auto entryIterator : classMap) {
        recordRefs.push_back(((classRecord*)entryIterator.second));
    }
    //sort by ascending alphabetical order
    sort(recordRefs.begin(), recordRefs.end(), [](const classRecord* lhs, const classRecord* rhs) {
        return lhs->lexeme < rhs->lexeme;
    });

    for(auto rec : recordRefs) {
        top = globalEnv;
        top = top->links.at({rec->lexeme,"class"});


        string klass = rec->lexeme;
        list<vector<pair<objectRecord*, string>>> attributesByClassInHierarchy{};
        vector<pair<objectRecord*, string>> attributes; //objectRecord and definingClass
        vector<string> hierarchyTraversalKeys; //need info about path we took to get back
        if(klass != "Object" && klass != "Int" && klass != "IO" && klass != "Bool" && klass != "String") {
            //top = top->links.at(make_pair(rec->lexeme, "class"));
            //we need every attribute for each class held by rec : recordRefs, so we first have to climb the the inheritance tree
            //up to but not including Object (has no attributes to print anyway, and is the root of the hierarchy)
            classRecord* currentClassRec = rec;
            classRecord* currentParentRec = classMap.at(rec->parent);
            hierarchyTraversalKeys.push_back(currentClassRec->lexeme);
            while(currentClassRec->lexeme != "Object") {
                currentParentRec = currentClassRec;
                if(classMap.find(currentClassRec->parent) != classMap.end()) {
                    currentClassRec = classMap.at(currentClassRec->parent);
                    hierarchyTraversalKeys.push_back(currentClassRec->lexeme);
                }
            }
            //traverse the hierarchy (starting one before Object) back to where we started, pushing back attributes as we encounter them
            for(int i = hierarchyTraversalKeys.size() - 2; i >= 0; --i) {
                currentClassRec = classMap.at(hierarchyTraversalKeys[i]);
                for(auto klasssIt : globalEnv->links.at(make_pair(currentClassRec->lexeme, "class"))->symTable) { //get all attributes for this class
                    if(klasssIt.second->kind == "attribute") {
                        attributes.push_back({(objectRecord*)klasssIt.second, currentClassRec->lexeme});
                    }
                }
                sort(attributes.begin(), attributes.end(), [](const pair<objectRecord*, string> lhs, const pair<objectRecord*, string> rhs) { //sort attributes in this class by encountered counter
                    return lhs.first->encountered < rhs.first->encountered;
                });
                attributesByClassInHierarchy.push_back(attributes);
                attributes.clear();

            }


        }

        //flatten the list of vectors for the attributes, can reuse the intermediary vector
        for(auto currentList : attributesByClassInHierarchy) {
            for(auto currentRec : currentList) {
                attributes.push_back(currentRec);
            }
        }

        //remove all selfs because we don't want to print them as attributes
        vector<pair<objectRecord*, string>> attributes2;
        for(auto attribute : attributes) {
            if(attribute.first->lexeme != "self") {
                attributes2.push_back(attribute);
            }
        }
        attributes = attributes2;
        rec->numAttributes = attributes.size();
    }
}
void populateParentMap() {
    parentMap = classMap;
    parentMap.erase("Object");
}

void printParentMap(ostream& out) {
    vector<Record*> recordRefs;
    for(auto entryIterator : parentMap) {
        recordRefs.push_back((entryIterator.second));
    }
    //sort by ascending alphabetical order
    sort(recordRefs.begin(), recordRefs.end(), [](const Record* lhs, const Record* rhs) {
        return lhs->lexeme < rhs->lexeme;
    });

    out << "parent_map\n" << recordRefs.size() << endl;
    for(auto rec : recordRefs) {
        classRecord* castedRec = (classRecord*)rec;
        out << castedRec->lexeme << endl;
        out << castedRec->parent << endl;
    }
}

map<string, map<string, pair<methodRecord*, string>>> implementationMap;
void populateImplementationMap() {
    vector<classRecord*> classRecs{};
    for(map<pair<string, string>, Record*>::iterator classRec = globalEnv->symTable.begin(); classRec != globalEnv->symTable.end(); classRec++) {
        classRecs.push_back((classRecord*)classRec->second);
    }
    //sort by ascending alphabetical order
    sort(classRecs.begin(), classRecs.end(), [](const classRecord* lhs, const classRecord* rhs) {
        return lhs->lexeme < rhs->lexeme;
    });

    //main loop
    list<methodRecord*> methodRecords{};
    for(classRecord* rec : classRecs) {
        //we print the inherited/overriden methods first, so we definitely need to start from top of hierarchy tree
        //also keep track of path so we can go back down the inheritance relations
        vector<string> inheritancePath = getInheritancePath(rec->lexeme);

        map<string, string> methodsMap; //key is method name, value is (most recent)defining class in the hierarchy.
        for(vector<string>::reverse_iterator klass = inheritancePath.rbegin(); klass != inheritancePath.rend(); klass++) {
            for(auto iter : (globalEnv->links.at(make_pair(*klass, "class")))->symTable) {
                if(iter.first.second == "method") {
                    methodsMap[iter.first.first] = *klass;
                }
            }
        }

        //<methodName, <mRecord*, definingClass>>
        map<string, pair<methodRecord*, string>> finalizedMethods;
        for(auto methodsIt : methodsMap) {
           finalizedMethods.insert(make_pair(methodsIt.first, make_pair((methodRecord*)globalEnv->links[{methodsIt.second,"class"}]->symTable[{methodsIt.first, "method"}], methodsIt.second)));
        }
        implementationMap.insert(make_pair(rec->lexeme, finalizedMethods));
    }
}

void printImplementationMap(ostream& out) {
    vector<string> classes;
    for(auto klass : implementationMap) {
        classes.push_back(klass.first);
    }
    sort(classes.begin(), classes.end());


    out << "implementation_map\n" << implementationMap.size() << endl;
    for(string klass : classes) {
        //Go up the hierarchy... last entry is most recent class (so last will always be Object)
        vector<string> inheritancePath = getInheritancePath(klass);
        //this is the output vector, in the required order
        vector<methodRecord*> methodsToPrint;
        map<string, int> methodNameToMethodRecByVectorPos; //need this for redefinitions/overriding. In order to maintain order,
        //it more or less keeps track of AN existence of <string> key but knows which position in vector<methodRecord*>methods
        //to overwrite (and keeps the same <int>
        for(int i = inheritancePath.size() - 1; i >= 0; --i) {
            int sizeBeginningOfIter = methodsToPrint.size(); //for incrementing an iterator later
            Environment* classEnv = globalEnv->links.at({inheritancePath[i], "class"});
            vector<methodRecord*> curClassMethodsToFilter = classEnv->getMethods();
            for(vector<methodRecord*>::iterator it = curClassMethodsToFilter.begin(); it != curClassMethodsToFilter.end(); it++){
                if(methodNameToMethodRecByVectorPos.find((*it)->lexeme) != methodNameToMethodRecByVectorPos.end()) { //it is in the map already
                    methodsToPrint[methodNameToMethodRecByVectorPos[(*it)->lexeme]] = *it; //replace it in the vector b/c we know its position in the vector
                }
                else {//not in the map, so it is new and not in the vector, so push_back
                    methodsToPrint.push_back(*it); //the first push_back gets put into methodsToPrint[sizeBeginningOfIter]
                }
            }
            sort(methodsToPrint.begin() + sizeBeginningOfIter, methodsToPrint.begin() + methodsToPrint.size(), [](const methodRecord* lhs, const methodRecord* rhs) {
                return lhs->encountered < rhs->encountered;
            });
            for(int j = sizeBeginningOfIter; j < methodsToPrint.size(); ++j) {
                methodNameToMethodRecByVectorPos[methodsToPrint[j]->lexeme] = j;
            }
        }

        out << klass << endl << methodsToPrint.size() << endl;
        for(methodRecord* method : methodsToPrint) {
            out << method->lexeme << endl;
            _method* treeNode = method->treeNode;
            out << treeNode->formalList.size() << endl;
            for(_formal* formal : treeNode->formalList) {
                out << formal->identifier.identifier << endl;
            }
            string mostRecentDefiningClass = implementationMap.at(klass).at(method->lexeme).second;
            out << mostRecentDefiningClass << endl;

            //print body expression of the method
            top = top->links.at({mostRecentDefiningClass, "class"})->links.at({method->lexeme, method->kind});
            out << *(method->treeNode->body);
            top = top->previous->previous;
        }
    }
}

bool conforms(string T1, string T2) {
    //TODO figure out a better way to do implement SELF_TYPE
    if(T1 == "SELF_TYPE") T1 = lookUpSelfType(top);
    if(T2 == "SELF_TYPE") T2 = lookUpSelfType(top);


    if(T1 == T2) {
        return true;
    }
    else if(T1 == "Object") {
        return T1 == T2;
    }
    while(T1 != T2) {
        if(T1 == "") { //TODO this is a bandaid. return false if it never got to a common parent
            return false;
        }
        if(classMap[T1]->parent == T2) {
            return true;
        }
        T1 = classMap[T1]->parent;
    }
}

/**
 * should be a set<string> but whatever, just need to iterate over all of them
 * @param typeChoices
 * @return
 */
string getLub(vector<string> typeChoices) {
    if (!typeChoices.size()) {
        return "Object";
    }
    else if(typeChoices.size() == 1) return typeChoices[0];

    map<string, vector<string>> inheritancePaths{{typeChoices[0],getInheritancePath(typeChoices[0])}};
    int minVecSize = inheritancePaths[typeChoices[0]].size();
    for (int i = 1; i < typeChoices.size(); ++i) {
        string current = typeChoices[i];
        inheritancePaths[current] = getInheritancePath(current);
        if(inheritancePaths[current].size() < minVecSize) {
            minVecSize = inheritancePaths[current].size();
        }
    }
    for(map<string, vector<string>>::iterator it = inheritancePaths.begin(); it != inheritancePaths.end(); it++){
        reverse(it->second.begin(), it->second.end());
    } //since getInheritancePath returns with Object at the end of the list

    int i = 0;
    string lub; //lub should always start off getting Object in the first iteration
    while(i < minVecSize) {
        string matchThis = inheritancePaths.begin()->second[i];
        //start on one past begin
        for(map<string, vector<string>>::iterator it = ++(inheritancePaths.begin()); it != inheritancePaths.end(); it++){
            if(it->second[i] != matchThis) {
                return lub;
            }
        }
        i++;
        lub = matchThis;
    }

    return lub;
}



