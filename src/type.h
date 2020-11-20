#ifndef COOLCOMPILERPROJECTALL_TYPE_H
#define COOLCOMPILERPROJECTALL_TYPE_H

#include <Environment.h>
#include <string>


using namespace std;
//Class map, implementation map, and parent map as described at
//https://dijkstra.eecs.umich.edu/eecs483/pa4.php and in the manual at https://dijkstra.eecs.umich.edu/eecs483/crm/Class%20definitions.html
extern map<string, Record*> classMap;
extern map<string, Record*> parentMap;


struct lessThanRecordForClassMap {
    inline bool operator() (const Record*& r1, const Record*& r2);
};
void populateClassMap();
void printClassMap(ostream& out);
void populateParentMap();
void printParentMap(ostream& out);



#endif //COOLCOMPILERPROJECTALL_TYPE_H
