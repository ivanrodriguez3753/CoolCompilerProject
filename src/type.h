#ifndef COOLCOMPILERPROJECTALL_TYPE_H
#define COOLCOMPILERPROJECTALL_TYPE_H

#include <Environment.h>
#include <string>


using namespace std;
//Class map, implementation map, and parent map as described at
//https://dijkstra.eecs.umich.edu/eecs483/pa4.php and in the manual at https://dijkstra.eecs.umich.edu/eecs483/crm/Class%20definitions.html
extern map<string, classRecord*> classMap; //key points to class Record
extern map<string, classRecord*> parentMap;
extern map<string, list<pair<methodRecord*, string>>> implementationMap;



void populateClassMap();
void printClassMap(ostream& out);
void populateParentMap();
void printParentMap(ostream& out);
void populateImplementationMap();
void printImplementationMap(ostream& out);
void buildBasicClassNodes();



#endif //COOLCOMPILERPROJECTALL_TYPE_H
