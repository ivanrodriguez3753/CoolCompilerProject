#include "syntaxTreeNodes.h"
#include <iostream>
using namespace std;


_node::_node(int l) : lineNo{l} {

}


_classNode::_classNode(_identifier id) :
    _node{0}, typeIdentifier{id}
{

}

_classNoInhNode::_classNoInhNode(_identifier id) :
    _classNode{id}
{
}

ostream &operator<<(ostream &os, const _classNode &c) {
    os << c.typeIdentifier;
    os << "no_inherits" << endl;
    os << c.featureList.size() << endl;
    for(auto feature : c.featureList) {
        if((_attributeNoInit*) feature) {
            os << *(_attributeNoInit*)feature;
        }

    }
    return os;
}

ostream& operator<<(ostream& os, const _identifier& i) {
    os << i.lineNo << endl;
    os << i.identifier << endl;
    return os;
}

ostream &operator<<(ostream &os, const _programNode &p) {
    os << p.classList.size() << endl;
    for(auto klass : p.classList) {
        os << *klass;
    }
    return os;
}

ostream &operator<<(ostream &os, const _attributeNoInit& a) {
    os << "attribute_no_init" << endl;
    os << a.identifier;
    os << a.typeIdentifier;
    return os;
}

_featureNode::_featureNode(int l, _identifier id, _identifier typeId) :
    _node{l}, identifier{id}, typeIdentifier{typeId}
{

}

_attributeNoInit::_attributeNoInit(int l, _identifier id, _identifier typeId) :
    _featureNode{l, id, typeId}
{

}
