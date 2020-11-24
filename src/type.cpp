#include "type.h"
#include "Environment.h"
#include <vector>
#include <iostream>
#include "syntaxTreeNodes.h"



map<string, classRecord*> classMap{};
/**
 * Parent map is kinda redundant, it's just a shallow copy without "Object"
 */
map<string, classRecord*> parentMap{};

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
        string klass = rec->lexeme;
        out << klass << endl;
        list<vector<objectRecord*>> attributesByClassInHierarchy{};
        vector<objectRecord*> attributes;
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
                for(auto klasssIt : top->links.at(make_pair(currentClassRec->lexeme, "class"))->symTable) { //get all attributes for this class
                    if(klasssIt.second->kind == "attribute") {
                        attributes.push_back((objectRecord*)klasssIt.second);
                    }
                }
                sort(attributes.begin(), attributes.end(), [](const objectRecord* lhs, const objectRecord* rhs) { //sort attributes in this class by encountered counter
                    return lhs->encountered < rhs->encountered;
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

        out << attributes.size() << endl;
        for(auto attribute : attributes) {
            if(attribute->initExpr) {
                out << "initializer\n";
                out << attribute->lexeme << endl << attribute->type << endl;
                out << *(attribute->initExpr);
            }
            else {
                out << "no_initializer\n";
                out << attribute->lexeme << endl << attribute->type << endl;
            }
        }

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

map<string, list<pair<methodRecord*, string>>> implementationMap;
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
        vector<string> inheritancePath{};
        classRecord* currentRec = rec;
        while(currentRec->parent != "") { //Object's parent is the empty string
            inheritancePath.push_back(currentRec->lexeme);
            currentRec = (classRecord*)globalEnv->symTable.at(make_pair(currentRec->parent, "class"));
        }
        inheritancePath.push_back(currentRec->lexeme);

        map<string, string> methodsMap; //key is method name, value is (most recent)defining class in the hierarchy.
        vector<string> orderInserted; //keep order in which we install these on the map. using map for faster lookup
        for(vector<string>::reverse_iterator klass = inheritancePath.rbegin(); klass != inheritancePath.rend(); klass++) {
            cout << *klass << "->";
            for(auto iter : (globalEnv->links.at(make_pair(*klass, "class")))->symTable) {
                if(iter.first.second == "method") {
                    methodsMap[iter.first.first] = *klass;
                    orderInserted.push_back(iter.first.first);
                }
            }
        }

        //put them in the map in the order they get printed
        list<pair<methodRecord*, string>> finalizedMethods{};
        for(vector<string>::iterator methodNameIt = orderInserted.begin(); methodNameIt != orderInserted.end(); methodNameIt++){
            //go to the env for the defining class, then search that for method with name pointed at by iterator
            finalizedMethods.push_back(make_pair((methodRecord*)globalEnv->links.at(make_pair(methodsMap.at(*methodNameIt), "class"))->symTable.at(make_pair(*methodNameIt, "method")), methodsMap.at(*methodNameIt)));
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
        list<pair<methodRecord*, string>> methods = implementationMap.at(klass);
        out << klass << endl << methods.size() << endl;
        for(pair<methodRecord*, string> method : methods) {
            out << method.first->lexeme << endl;
//            if(method.second != "Object" && method.second != "Int" && method.second != "Bool" && method.second != "String" && method.second != "IO") { //not a basic class
                _method* treeNode = method.first->treeNode;
                out << treeNode->formalList.size() << endl;
                for(_formal* formal : treeNode->formalList) {
                    out << formal->identifier.identifier << endl;
                }
//            }
//            else { //cases for basic classes, since they are not in the AST
//                _method treeNode
//            }
            out << method.second << endl;

            //print body expression of the method
//            if(method.second != "Object" && method.second != "Int" && method.second != "Bool" && method.second != "String" && method.second != "IO") {
                out << *(method.first->treeNode->body);
//            }
//            else {
//                //Bool and Int don't have any methods. Just branches for Object/String/IO
//                //They are also gonna come in order so no worries about ordering
//                if(method.second == "Object"){
//                    out << ((_method*)Object_class->featureList.front())->body;
//                    //this is bad but just take off the list and append at the end
//                    _method* temp = (_method*)Object_class->featureList.front();
//                    Object_class->featureList.pop_front();
//                    Object_class->featureList.push_back(temp);
//
//                }
//                else if(method.second == "String") {
//
//                }
//
//            }


        }
    }
}