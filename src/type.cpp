#include "type.h"
#include "Environment.h"
#include <vector>
#include <iostream>



map<string, Record*> classMap{};
/**
 * Parent map is kinda redundant, it's just a shallow copy without "Object"
 */
map<string, Record*> parentMap{};


void populateClassMap() {
    for(map<pair<string, string>, Record*>::iterator entryIterator = globalEnv->symTable.begin(); entryIterator != globalEnv->symTable.end(); entryIterator++) {
        //entryIterator.first.first is the lexeme (entryIterator.first.second is the kind)
        //entryIterator.second is a Record, so pass its address
        string first = entryIterator->first.first;
        Record* second = (entryIterator->second);
        classMap.insert(make_pair(first, second));
    }
}
void printClassMap(ostream& out) {
    vector<Record*> recordRefs;
    for(auto entryIterator : classMap) {
        recordRefs.push_back((entryIterator.second));
    }
    //sort by ascending alphabetical order
    sort(recordRefs.begin(), recordRefs.end(), [](const Record* lhs, const Record* rhs) {
        return lhs->lexeme < rhs->lexeme;
    });

    out << "class_map\n" << recordRefs.size() << endl;
    for(auto rec : recordRefs) {
        string klass = rec->lexeme;
        out << klass << endl;
        vector<objectRecord*> attributes;
        if(klass != "Object" && klass != "Int" && klass != "IO" && klass != "Bool" && klass != "String") {
            top = top->links.at(make_pair(rec->lexeme, "class"));
            for(map<pair<string, string>, Record*>::iterator entryIt = top->symTable.begin(); entryIt != top->symTable.end(); entryIt++) {
                if(entryIt->first.second == "attribute") {
                    attributes.push_back((objectRecord*)entryIt->second);
                }
            }
            top = top->previous;
        }
        sort(attributes.begin(), attributes.end(), [](const objectRecord* lhs, const objectRecord* rhs) {
           return lhs->encountered < rhs->encountered;
        });
        //TODO attributes from parent classes, add to total attributes
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