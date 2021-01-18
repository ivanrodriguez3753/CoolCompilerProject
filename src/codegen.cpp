#include <sstream>
#include "syntaxTreeNodes.h"
#include "codegen.h"
#include "type.h"

vector<string> code;

/**
 * Activation Record layout (for convenience, I show a downward growing stack):
 *      Temporaries
 *      Return address
 *
 */

const string r0 = "r0";
const string r1 = "r1";
const string r2 = "r2";
const string r3 = "r3";
const string r4 = "r4";
const string r5 = "r5";
const string r6 = "r6";
const string r7 = "r7";
const string sp = "sp";
const string fp = "fp";
const string ra = "ra";

const string& self_reg = r0;
const string& acc_reg = r1;
const string& temp_reg = r2;
const string new_frame = "\tmov fp <- sp";
const string rreturn = "\treturn";
const string newSuffix = ".new";
const string vTableSuffix = ".vtable";

const int classTagOffset = 0;
const int objectSizeOffset = 1;
const int vtablePointerOffset = 2;
const int firstAttributeOffset = 3;

void li(string r, int i) {
    code.push_back("\tli " + r + " <- " + to_string(i));
}
void li(string r, string label) {
    code.push_back("\tli " + r + " <- " + label);
}
void la(string r, string regOrLabel) {
    code.push_back("\tla " + r + " <- " + regOrLabel);
}
void st(string r_base, int offset, string r_value) {
    code.push_back("\tst " + r_base + "[" + to_string(offset) + "] <- " + r_value);
}
void call(string r) {
    code.push_back("\tcall " + r);
}
void push(string r) {
    code.push_back("\tpush " + r);
}
void pop(string r) {
    code.push_back("\tpop " + r);
}
void mov(string r_dest, string r_source) {
    code.push_back("\tmov " + r_dest + " <- " + r_source);
}
void syscall(string syscall) {
    code.push_back("\tsyscall " + syscall);
}
void sub(string r_dest, string r_subtractFrom, string r_subtractThis) {
    code.push_back("\tsub " + r_dest + " <- " + r_subtractFrom + " " + r_subtractThis);
}
void add(string r_dest, string r_addTo, string r_addThis) {
    code.push_back("\tadd " + r_dest + " <- " + r_addTo + " " + r_addThis);
}
/**
 * caller conventions
 * @param klass
 * @param subroutine
 */
void callerCallAndReturnSequence(string klass, string subroutine) {
    //COMMENT
    code.push_back("\t;;Caller calling conventions;;;;;;;");
    push(fp);
    push(self_reg);
    la(temp_reg, klass + "." + subroutine);
    call(temp_reg);

    //COMMENT
    code.push_back("\t;;Caller return conventions;;;;;;;;");
    pop(self_reg);
    pop(fp);
}

/**
 *
 * @param method
 * @param stackRoomForTemps default to 1 because all Klass..new constructors only need 1 temp
 */
void calleeCallSequence(string method, int stackRoomForTemps = 1) {
    //COMMENT
    code.push_back(";;Callee calling conventions;;;;;;;;;;;;;;");
    code.push_back(new_frame);
    if(method != newSuffix) pop(self_reg);
    //COMMENT
    code.push_back(";; stack room for temps: " + to_string(stackRoomForTemps) + ";;;;;;;;;;");
    li(temp_reg, stackRoomForTemps);
    sub(sp, sp, temp_reg);
    push(ra);
}

/**
 * as mentioned before, all ..new stackFrames only need room for 1 temp
 * @param klass
 * @param method
 * @param stackRoomForTemps
 */
void calleeReturnSequence(string klass, string method, int stackRoomForTemps = 1) {
    code.push_back(klass + "." + method + "." + "end:");
    //COMMENT
    code.push_back("\t;;callee return conventions;;;;;;;");
    if(method == newSuffix) mov(acc_reg, self_reg);
    pop(ra);
    li(temp_reg, stackRoomForTemps);
    add(sp, sp, temp_reg);
    code.push_back(rreturn);
}


/**
 * used only in constructors and copy()
 * @param klass
 * @return heapFrameSize
 */
int allocAndStoreAttributesAndInitializers(classRecord* klass) {
    //COMMENT
    code.push_back("\t;;alloc then store classTag->objectSize->vtable->attributes in that order;;;;;;;;;;");

    //every heap frame follows this order: classTag, objectSize, vtablePointer, attributes...
    int heapFrameSize;
    if(klass->lexeme == "String" || klass->lexeme == "Int" || klass->lexeme == "Bool") {
        heapFrameSize = firstAttributeOffset + 1; //3 necessary frames and 1 for the raw, unboxed value
    }
    else {
        heapFrameSize = firstAttributeOffset + classMap.at(klass->lexeme)->numAttributes;
    }

    li(self_reg, heapFrameSize);
    alloc(self_reg, self_reg);

    int curOffset = 0;
    li(temp_reg, klass->encountered); //using classRecord.encountered as class tags
    st(self_reg, curOffset++, temp_reg);
    li(temp_reg, heapFrameSize);
    st(self_reg, curOffset++, temp_reg);
    la(temp_reg, klass->lexeme + '.' + vTableSuffix);
    st(self_reg, curOffset++, temp_reg);

    //COMMENT
    code.push_back("\t;; now start attributes. make new default objects;;;;;;;;;;;");
    if(klass->lexeme == "String" || klass->lexeme == "Int" || klass->lexeme == "Bool") {
        //COMMENT
        code.push_back("\t;;this is 1 of 3 basic classes. Only one attribute, which is the unboxed raw");
        pair<string, string> attr{"raw"+klass->lexeme, klass->lexeme};
        code.push_back("\t;;self[" + to_string(curOffset) + "] holds attr " + attr.first + " (" + attr.second + "), DON'T construct a new Bool. Just save raw value");

        if(klass->lexeme == "Int" || klass->lexeme == "Bool") {
            li(acc_reg, 0); //0 is default for Bool and Int (assembly types and Cool types)
        }
        else if(klass->lexeme == "String") {
            la(acc_reg, "the.empty.string");
        }
        st(self_reg, curOffset++, acc_reg);
    }
    else {
        for(objectRecord* attr : classMapOrdered.at(klass->lexeme)) {
            //COMMENT
            code.push_back("\t;;self[" + to_string(curOffset) + "] holds attr " + attr->lexeme + " (" + attr->type + "), so construct a new " + attr->type);

            callerCallAndReturnSequence(attr->type, newSuffix);
            st(self_reg, curOffset++, acc_reg);
        }
        curOffset = firstAttributeOffset; //start at beginning of attributes
        for(objectRecord* attr : classMapOrdered.at(klass->lexeme)) {
            //COMMENT
            string comment = "\t;;self[" + to_string(curOffset) + "] " + attr->lexeme + " (" + attr->type + ") initializer ";
            if(attr->initExpr) {
                //COMMENTS
                code.push_back(comment);
                code.push_back("\t;;new " + attr->initExpr->exprType + " for initializer expression;;;;;;;;;");
                callerCallAndReturnSequence(attr->type, newSuffix);
                //COMMENT
//if(attr->type == "Int") {//TODO: TEMPORARY. NEEDS TO WORK FOR ALL TYPES
                //COMMENT
                code.push_back("\t;;evaluate " + attr->type + " expression and store;;;;;;;;;;;;;;;;;");
                attr->initExpr->codeGen();

                //COMMENT
                code.push_back("\t;;store result of creating object from initializer expression;;;;;;;;;;");
                st(self_reg, curOffset, acc_reg);
//}

            }
            else {
                //COMMENT
                comment += "-- none";
                code.push_back(comment);
            }
            ++curOffset;
        }
    }

    return heapFrameSize;


}

/**
 * hard coded
 */
void genStart() {
    code.push_back("start:");
    push(fp);
    la(temp_reg, "Main" + newSuffix);
    call(temp_reg);

    push(fp);
    push(acc_reg);
    la(temp_reg, "Main.main");
    call(temp_reg);

}


/**
 * return pointer to base of newly allocated memory
 * @param r_dest which register to save the pointer in
 * @param r_numWords register holding an integer
 */
void alloc(string r_dest, string r_numWords) {
    code.push_back("\talloc " + r_dest + " " + r_numWords);
}






map<string, list<objectRecord*>> classMapOrdered;
map<string, list<pair<methodRecord*, string>>> implementationMapOrdered;


/***throwing all codegen stuff in here, despite some coming from syntaxTreeNodes.h
 */
void populateClassMapOrdered() {
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

        list<objectRecord*> insertThis;
        for(auto attribute : attributes) {

            if(attribute.first->lexeme == "self") continue; //i don't think self needs to be printed, or else it'd be printed once per class in hierarchy
            //make sure we're on the right symbol table for each attribute
            top = globalEnv->links.at({attribute.second ,"class"});
            insertThis.push_back((objectRecord*)(top->symTable.at({attribute.first->lexeme, "attribute"})));
        }
        classMapOrdered.insert({klass, list<objectRecord*>(insertThis.begin(), insertThis.end())});


        top = top->previous;
    }

}
void populateImplementationMapOrdered() {
    vector<string> classes;
    for(auto klass : implementationMap) {
        classes.push_back(klass.first);
    }
    sort(classes.begin(), classes.end());


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

        for(methodRecord* method : methodsToPrint) {
            string mostRecentDefiningClass = implementationMap.at(klass).at(method->lexeme).second;
            implementationMapOrdered[klass].push_back({method, mostRecentDefiningClass});
        }
    }
}
static int stringCounter = 1;
void _program::genVTables() {
    populateImplementationMapOrdered();
    for(map<string, classRecord*>::iterator klassIt = classMap.begin(); klassIt != classMap.end(); ++klassIt) {
        const string& klass = klassIt->first;

        //COMMENT
        code.push_back(";;start " + klass + "'s vtable;;;;;;;;;;;;;;;;;;;;;;;;;;");

        //print Klass..vtable:
        string vTableLbl = klass + "..vtable:";
        code.push_back(vTableLbl);

        /**
         * Every vtable for Klass has:
         *  constant stringX
         *  constant Klass..new
         *
         *Then we print the rest of the available methods as
         *  constant LastDefiningClass.methodName
         */

        //print label for string constant which holds class name
        //internal names are generated with a static counter
        code.push_back("\tconstant string" + to_string(stringCounter++));

        //label for constructor
        const string ctrLbl = "\tconstant " + klass + newSuffix;
        code.push_back(ctrLbl); //constructor "klass..new" is always first routine in klass..vtable:

        for(pair<methodRecord*, string> method : implementationMapOrdered.at(klass)) {
            const string vTableMethodlbl = method.second + "." + method.first->lexeme;
            code.push_back("\tconstant " + vTableMethodlbl);
        }
    }
}

void _program::genConstructors() {
    fillInClassAttributeNum(); //make this better hahahaha copied and pasted from printClassMap
    populateClassMapOrdered();
    for (map<string, classRecord *>::iterator klassIt = classMap.begin(); klassIt != classMap.end(); ++klassIt) {
        //COMMENT
        code.push_back(";;start " + klassIt->first + "'s constructor;;;;;;;;;;;;;;;;;;;;;");

        //print the label Klass..new
        code.push_back(klassIt->first + newSuffix);


        //COMMENT
        code.push_back("\t;;calling convention stuff: create new activation record;;;;;;;;;;;");
        code.push_back(new_frame);



        //COMMENT
        code.push_back("\t;; get size needed for heapFrame, then create heapFrame and save pointer;;;;;;;;;;;");

        //load heapFrame size into a register, which we know statically from numAttributes
        //*p -> heap
        //[vtable, attributes... ]

        li(self_reg, klassIt->second->numAttributes + 1);
        //+1 for the vtable pointer




        //ask OS for space in the heap
        alloc(self_reg, self_reg);

        //COMMENT
        code.push_back("\t;; store attributes (includes vtable pointer!);;;;;;;;;");

        //store attributes (vtable pointer is an attribute in this context)
        la(temp_reg, klassIt->first + "..vtable");
        st(self_reg, vtablePointerOffset, temp_reg);

        //now the actual attributes...
        int i = 1;
        for(objectRecord* attribute : classMapOrdered.at(klassIt->first)) {
            //COMMENT
            code.push_back("\t;; loading and calling constructor " + attribute->type + newSuffix +  " for attribute " + attribute->lexeme);

            //if this object has an initialization expression
            if(attribute->initExpr) {
                push(fp);
                push(self_reg);

                la(temp_reg, attribute->type + newSuffix);
                call(temp_reg);

                pop(self_reg);
                pop(fp);



                //TODO better way of accessing the thing in the conditional above??
                code.push_back("\t;;placeholder cgen for initializer expression");



                st(self_reg, i++, acc_reg); //stores result of evaluating initializer expression

            }
        }

        //COMMENT
        code.push_back("\t;;copy self pointer into accumulator;;;;;;;");
        mov(acc_reg, self_reg);

        //COMMENT
        code.push_back("\t;;clean up the stack;;;;;;;");
        pop(ra);
        pop(fp);
        code.push_back(rreturn);
    }
}

void genConstructors2() {
    fillInClassAttributeNum();
    populateClassMapOrdered();

    for(map<string, list<objectRecord*>>::const_iterator klassIt = classMapOrdered.begin(); klassIt != classMapOrdered.end(); ++klassIt) {
        //COMMENT
        code.push_back(";;start " + klassIt->first + "'s constructor;;;;;;;;;;;;;;;;;;;;;");

        //print the label Klass..new
        code.push_back(klassIt->first + '.' + newSuffix + ':');

        calleeCallSequence(newSuffix);
        allocAndStoreAttributesAndInitializers(classMap.at(klassIt->first));
        calleeReturnSequence(klassIt->first, newSuffix);





    }
}

void _program::genMethods() {

}


/**
 * print vtables, constructors, methods, and start label
 * @param instructions
 */
void _program::codeGen() {
    genVTables();
    genConstructors2();
    genStart();

    actuallyPrint();
}

/**
* an integer node always makes an Integer assembly object. As noted elsewhere,
* 0,1,2 are for classTag, objectSize, and Klass..vtable. Then attributes start.
* For an integer assembly object, the only "attribute" is the raw int value.
* So it is at position 3.
*/
void _integer::codeGen() {
    li(temp_reg, value);
    st(acc_reg, firstAttributeOffset, temp_reg);
}

void _bool::codeGen() {
    li(temp_reg, value);
    st(acc_reg, firstAttributeOffset, temp_reg);
}
