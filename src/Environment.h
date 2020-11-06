#ifndef COOLCOMPILERPROJECTALL_ENVIRONMENT_H
#define COOLCOMPILERPROJECTALL_ENVIRONMENT_H

#include <string>
#include <map>
using namespace std;

class Record {
public:
    string lexeme;
    int lineNo;
    string kind;

    Record(string lex, int l, string k);
};

class Environment {
private:
    /**
     * methods and attributes can have the same identifier, so use a pair as key <identifier, kind>
     * For example, <"someName", "attribute"> is different from <"someName", "method">
     */
    map<pair<string, string>, Record> symTable;

public:
    Environment* previous;
    map<pair<string, string>, Environment*> links;

    Environment(Environment* prev);

    void install(pair<string, string>key, Record rec);
    Record get(pair<string, string> key);

    void reset();
};

extern Environment* globalEnv;
extern Environment* top;

#endif //COOLCOMPILERPROJECTALL_ENVIRONMENT_H
