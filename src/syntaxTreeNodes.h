#ifndef COOLCOMPILERPROJECTALL_SYNTAXTREENODES_H
#define COOLCOMPILERPROJECTALL_SYNTAXTREENODES_H

#include <list>
#include <string>
#include "parseTreeNodes.h"

using namespace std;

class _node;
class _programNode;
class _classNode;
class _classNoInhNode;
class _classInhNode;
class _featureNode;
class _identifier;
/**
 * Builds abstract syntax tree out of the parse tree
 * @param root
 * @return
 */

class _node {
public:
    int lineNo;
    _node(int l);
};

class _identifier : public _node{
public:
    string identifier;

    _identifier(int l, string id) : _node{l}, identifier{id} {}

    friend ostream& operator<<(ostream& os, const _identifier& i);
};

/**
 * A Cool program is merely a list of classes
 */
class _programNode : public _node {
public:
    list<_classNode*> classList;
    _programNode(int l) : _node{l} {}

    friend ostream& operator<<(ostream& os, const _programNode& p);
};

class _classNode : public _node {
public:
    _identifier typeIdentifier;
    list<_featureNode*> featureList;


    friend ostream& operator<<(ostream& os, const _classNode& c);
    _classNode(_identifier id);
};

class _classNoInhNode : public _classNode {
public:
    friend ostream& operator<<(ostream& os, const _classNode& c);

    _classNoInhNode(_identifier id);
};

class _classInhNode : public _classNode {
public:
    _identifier superClassIdentifier;

    friend ostream& operator<<(ostream& os, const _classNode& c);
};

class _featureNode : public _node {
public:
    _identifier identifier;
    _identifier typeIdentifier;

    _featureNode(int l, _identifier id, _identifier typeId);
};

class _attributeNoInit : public _featureNode {
public:
    _attributeNoInit(int l, _identifier id, _identifier typeId);

    friend ostream& operator<<(ostream& os, const _attributeNoInit& a);
};




#endif //COOLCOMPILERPROJECTALL_SYNTAXTREENODES_H
