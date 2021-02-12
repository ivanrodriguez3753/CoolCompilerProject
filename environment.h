#ifndef COOLCOMPILERPROJECT_ENVIRONMENT_H
#define COOLCOMPILERPROJECT_ENVIRONMENT_H

#include <string>
#include <map>

using namespace std;

class rec;
class classRec;
class methodRec;
class letCaseRec;
class objRec;
class env;
class globalEnv;
class classEnv;
class methodEnv;
class letCaseEnv;

/**
 * Base class for all record types
 */
class rec {
    string id;
};

class classRec : public rec{
public:
    classEnv* link;

    const int numAttr;
    const int numMethods;

};
class methodRec : public rec {
public:
    methodEnv* link;

    const int numArgs;
};
class letCaseRec : public rec {
public:
    letCaseEnv* link;
    const int numLocals;
};

class objRec : public rec {
public:

    /**
     * Used for offsetting off sp[i >= firstAttrOffset] for attributes,
     * fp[i >= 0] for method arguments, or fp[i < 0] for local variables.
     */
    const int localOffset; //self offset for attributes, fp[i >= 0] for method arguments, fp[i < 0] for local variables
};


class env {
protected:
    map<string, rec*> symTable;
    env* prev;
public:
    virtual rec* getRec(const string& key) const = 0;
};

class globalEnv : public env {
public:
    classRec* getRec(const string& key) const override {
        return (classRec*)symTable.at(key);
    }
};
class classEnv : public env {
public:
    methodRec* getRec(const string& key) const override {
        return (methodRec*)symTable.at(key);
    }

};
class methodEnv : public env {
    objRec* getRec(const string& key) const override {
        return (objRec*)symTable.at(key);
    }
};
class letCaseEnv : public env {
    objRec* getRec(const string& key) const override {

    }
};


#endif //COOLCOMPILERPROJECT_ENVIRONMENT_H
