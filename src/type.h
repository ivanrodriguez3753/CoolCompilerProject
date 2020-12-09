#ifndef COOLCOMPILERPROJECTALL_TYPE_H
#define COOLCOMPILERPROJECTALL_TYPE_H

#include <Environment.h>
#include <string>


using namespace std;
//Class map, implementation map, and parent map as described at
//https://dijkstra.eecs.umich.edu/eecs483/pa4.php and in the manual at https://dijkstra.eecs.umich.edu/eecs483/crm/Class%20definitions.html
extern map<string, classRecord*> classMap; //key points to class Record
extern map<string, classRecord*> parentMap;


//<className, <methodName, <methodRecord*, mostRecentDefiningClass>>>
extern map<string, map<string, pair<methodRecord*, string>>> implementationMap;



void populateClassMap();
void printClassMap(ostream& out);
void populateParentMap();
void printParentMap(ostream& out);
void populateImplementationMap();
void printImplementationMap(ostream& out);
void buildBasicClassNodes();

/**
 * Returns true iff T1 <= T2 as described in the Cool reference manual
 * Definition 4.1 (Conformance) Let 𝙰,𝙲, and 𝙿 be types.
    𝙰≤𝙰 for all types A
    if C inherits from P, then 𝙲≤𝙿
    if 𝙰≤𝙲 and 𝙲≤𝙿 then 𝙰≤𝙿
    Because Object is the root of the class hierarchy, it follows that 𝙰≤𝙾𝚋𝚓𝚎𝚌𝚝 for all types 𝙰.
 * @param T1
 * @param T2
 * @return
 */
bool conforms(string T1, const string T2);

/**
 * Get least upper bound for the set of classes.
 * @param typeChoices
 * @return
 */
string getLub(vector<string> typeChoices);
vector<string> getInheritancePath(string klass);

void printAndPush(pair<int,string>);

class typeError_exception : public exception {
public:
    int lineNo;
    string error;
    typeError_exception(pair<int, string> e) : lineNo{e.first}, error{e.second} {
        cerr << "ERROR: " + to_string(lineNo) + ", " + error << endl;
    }
};

#endif //COOLCOMPILERPROJECTALL_TYPE_H
