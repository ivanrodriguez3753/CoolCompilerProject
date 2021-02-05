#include <sstream>
#include <stack>
#include "syntaxTreeNodes.h"
#include "codegen.h"
#include "type.h"

vector<string> code;


const string emptyStringLabel = "the.empty.string";
const string abortLabel = "abort";
const string substrLabel = "substr";
const string is_voidLabel = "is_void";
/**
 * Initialize with misc global string constants things we need to hardcode
 */
vector<string> globalStringConstants{
    emptyStringLabel + ':',
        "\tconstant \"\"",
    abortLabel + ':',
        "\tconstant \"abort!\\n\"",
    substrLabel + ':',
        "\tconstant \"ERROR: 0: Exception: String.substr out of range\\n\"",
    is_voidLabel + ':',
        "\tconstant \"ERROR: Dispatch on void\\n\""
};




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

const string exitt = "exit";

const string& self_reg = r0;
const string& acc_reg = r1;
const string& temp_reg = r2;
const string new_frame = "\tmov fp <- sp";
const string rreturn = "\treturn";
const string newSuffix = ".new";
const string vTableSuffix = ".vtable";

bool isLHS = true; //TODO: stop passing around globals for codeGen

string currentClass;
string currentMethod;

const int classTagOffset = 0;
const int objectSizeOffset = 1;
const int vtablePointerOffset = 2;
const int firstAttributeOffset = 3;

const int classNameOffset = 0;
const int newPointerOffset = 1;
const int firstMethodOffset = 2;

const int firstArgOffset = 2;

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
void ld(string r_dest, string r_base, int offset) {
    code.push_back("\tld " + r_dest + " <- " + r_base +  '[' + to_string(offset) + ']');
}
void mul(string r_dest, string r_addTo, string r_addThis) {
    code.push_back("\tmul " + r_dest + " <- " + r_addTo + " " + r_addThis);
}
void div(string r_dest, string r_addTo, string r_addThis) {
    code.push_back("\tdiv " + r_dest + " <- " + r_addTo + " " + r_addThis);
}
void bnz(string r_source, string label) {
    code.push_back("\tbnz " + r_source + ' ' + label);
}
void bz(string r_source, string label) {
    code.push_back("\tbz " + r_source + ' ' + label);
}
void jmp(string label) {
    code.push_back("\tjmp " + label);
}
/**
 * return pointer to base of newly allocated memory
 * @param r_dest which register to save the pointer in
 * @param r_numWords register holding an integer
 */
void alloc(string r_dest, string r_numWords) {
    code.push_back("\talloc " + r_dest + " " + r_numWords);
}

/**
 * caller conventions for calling a constructor.
 * @param klass
 * @param subroutine
 */
void callerConstructorCallAndReturnSequence(string klass) {
    //COMMENT
    code.push_back("\t;; new " + klass + " Caller calling conventions;;;;;;;");
    push(fp);
    push(self_reg);
    la(temp_reg, klass + "." + newSuffix);
    call(temp_reg);

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
    code.push_back("\t;;Callee calling conventions;;;;;;;;;;;;;;");
    code.push_back(new_frame);
    if(method != newSuffix) pop(self_reg);
    //COMMENT
    code.push_back("\t;; stack room for temporaries: " + to_string(stackRoomForTemps) );
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
    code.push_back("\t;; now start attributes. make new default void objects, except on String/Int/Bool;;;;;;;;;;;");
    const int oldCurOffset = curOffset;

    //String/Int/Bool can never be void objects. They are default initialized
    if(klass->lexeme == "String" || klass->lexeme == "Int" || klass->lexeme == "Bool") {
        //not looping over attributes because these three classes only have one attribute (in addition to the required ones): unboxed raw
        //COMMENT
        code.push_back("\t;;self[" + to_string(firstAttributeOffset) + "] holds unboxed raw " + klass->lexeme);
        if(klass ->lexeme == "Bool" || klass->lexeme == "Int") {
            li(acc_reg, 0);
        }
        else {//String
            la(acc_reg, emptyStringLabel);
        }
        st(self_reg, firstAttributeOffset, acc_reg);
    }
    else {//everything else starts as void object, even if it has an initExpr
        for(objectRecord* attr : classMapOrdered.at(klass->lexeme)) {
            //COMMENT
            code.push_back("\t;;self[" + to_string(curOffset) + "] holds attr " + attr->lexeme + " (" + attr->type + ')');

            //String/Int/Bool can never be void objects
            if(attr->type == "String" || attr->type == "Bool" || attr->type == "Int") {
                callerConstructorCallAndReturnSequence(attr->type);
            }//everything else starts off as void, even if it has an initializer
            //a pointer to a value of zero is a void object
            else {
                li(acc_reg, 0);
            }
            st(self_reg, curOffset++, acc_reg);
        }

        //COMMENT
        code.push_back("\t;;now handle initializers for each attribute");
        curOffset = oldCurOffset;
        for(objectRecord* attr : classMapOrdered.at(klass->lexeme)) {
            string comment = "\t;; self[" + to_string(curOffset) + "] initializer";
            if(attr->initExpr) {
                //COMMENT
                code.push_back(comment);
                callerConstructorCallAndReturnSequence(attr->initExpr->exprType);
                attr->initExpr->codeGen();
                st(self_reg, curOffset++, acc_reg);
            }
            else {//no initializer
                comment += " --none";
                //COMMENT
                code.push_back(comment);
                curOffset++;
            }
        }
    }

//    if(klass->lexeme == "String" || klass->lexeme == "Int" || klass->lexeme == "Bool") {
//        //COMMENT
//        code.push_back("\t;;this is 1 of 3 basic classes. Only one attribute, which is the unboxed raw");
//        pair<string, string> attr{"raw"+klass->lexeme, klass->lexeme};
//        code.push_back("\t;;self[" + to_string(curOffset) + "] holds attr " + attr.first + " (" + attr.second + "), DON'T construct a new Bool/Int. Just save raw value");
//
//        callerConstructorCallAndReturnSequence(klass->lexeme);
//        if(klass->lexeme == "Int" || klass->lexeme == "Bool") {
//            li(acc_reg, 0); //0 is default for Bool and Int (assembly types and Cool types)
//        }
//        else if(klass->lexeme == "String") {
//            la(acc_reg, emptyStringLabel);
//        }
//        st(self_reg, curOffset++, acc_reg);
//    }
//    else { //any class that is not String/Int/Bool
//        for(objectRecord* attr : classMapOrdered.at(klass->lexeme)) {
//            //COMMENT
//            code.push_back("\t;;self[" + to_string(curOffset) + "] holds attr " + attr->lexeme + " (" + attr->type + "), so construct a new " + attr->type);
//
//            li(acc_reg, 0);
//            st(self_reg, curOffset++, acc_reg);
//        }
//        curOffset = firstAttributeOffset; //start at beginning of attributes
//        for(objectRecord* attr : classMapOrdered.at(klass->lexeme)) {
//            //COMMENT
//            string comment = "\t;;self[" + to_string(curOffset) + "] " + attr->lexeme + " (" + attr->type + ") initializer ";
//            if(attr->initExpr) {
//                //COMMENTS
//                code.push_back(comment);
//                code.push_back("\t;;new " + attr->initExpr->exprType + " for initializer expression;;;;;;;;;");
//                callerConstructorCallAndReturnSequence(attr->type);
//
//                //COMMENT
//                code.push_back("\t;;evaluate " + attr->type + " expression and store;;;;;;;;;;;;;;;;;");
//                attr->initExpr->codeGen();
//
//                //COMMENT
//                code.push_back("\t;;store result of creating object from initializer expression;;;;;;;;;;");
//                st(self_reg, curOffset, acc_reg);
//
//            }
//            else {
//                //COMMENT
//                comment += "-- none";
//                code.push_back(comment);
//            }
//            ++curOffset;
//        }
//    }

    return heapFrameSize;
}

/**
 * hard coded
 */
void _program::genStart() {
    //COMMENT
    code.push_back(";;ENTRY POINT HERE;;;;;;;;;;;;;;;;");

    code.push_back("start:");
    push(fp);
    la(temp_reg, "Main." + newSuffix);
    call(temp_reg);

    push(fp);
    push(acc_reg);
    la(temp_reg, "Main.main");
    call(temp_reg);
    syscall(exitt);

}









map<string, list<objectRecord*>> classMapOrdered;

/**
 * <string_class, list<pair<methodRecord*, string_mostRecentDefiningClass>>>
 */
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
int _string::stringCounter = 1;
void _program::genVTablesAndGlobalStringConstantsClassNames() {
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
        string internalName = "string" + to_string(_string::stringCounter++);
        code.push_back("\tconstant " + internalName);

        //for tacking onto code at some point. saving internal names as we generate them
        globalStringConstants.push_back(internalName + ':');
        globalStringConstants.push_back("\tconstant \"" + klass + '\"');

        //label for constructor
        const string ctrLbl = "\tconstant " + klass + '.' + newSuffix;
        code.push_back(ctrLbl); //constructor "klass..new" is always first routine in klass..vtable:

        for(pair<methodRecord*, string> method : implementationMapOrdered.at(klass)) {
            const string vTableMethodlbl = method.second + "." + method.first->lexeme;
            code.push_back("\tconstant " + vTableMethodlbl);
        }
    }
}

void _program::genConstructors() {

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

//map<string, vector<string>> hardcodedStarterObjectMethodBodies{


//};

void _program::setUpHardcodeStringStream(int i, stringstream& s) {
    s.clear();
    switch(i) {
        case 0: //Object.abort
            s = stringstream("    la r1 <- abort\n"
                             "    syscall IO.out_string\n"
                             "    syscall exit");
            break;
        case 1: //Object.copy
            s = stringstream("    ld r2 <- r0[1]\n"
                             "    alloc r1 r2\n"
                             "    push r1\n"
                             "l1:\n"
                             "    bz r2 l2\n"
                             "    ld r3 <- r0[0]\n"
                             "    st r1[0] <- r3\n"
                             "    li r3 <- 1\n"
                             "    add r0 <- r0 r3\n"
                             "    add r1 <- r1 r3\n"
                             "    li r3 <- 1\n"
                             "    sub r2 <- r2 r3\n"
                             "    jmp l1\n"
                             "l2:\n"
                             "    ;; done with Object.copy loop\n"
                             "    pop r1");
            break;
        case 2: //Object.type_name
            s = stringstream("    push fp\n"
                             "    push r0\n"
                             "    la r2 <- String..new\n"
                             "    call r2\n"
                             "    pop r0\n"
                             "    pop fp\n"
                             "    ;; obtain vtable for self object\n"
                             "    ld r2 <- r0[2]\n"
                             "    ;; look up type name at offset 0 in vtable\n"
                             "    ld r2 <- r2[0]\n"
                             "    st r1[3] <- r2");
            break;
        case 3: //IO.in_int
            s = stringstream("    push fp\n"
                             "    push r0\n"
                             "    la r2 <- Int..new\n"
                             "    call r2\n"
                             "    pop r0\n"
                             "    pop fp\n"
                             "    mov r2 <- r1\n"
                             "    syscall IO.in_int\n"
                             "    st r2[3] <- r1\n"
                             "    mov r1 <- r2");
            break;
        case 4: //IO.in_string
            s = stringstream("    push fp\n"
                             "    push r0\n"
                             "    la r2 <- String..new\n"
                             "    call r2\n"
                             "    pop r0\n"
                             "    pop fp\n"
                             "    mov r2 <- r1\n"
                             "    syscall IO.in_string\n"
                             "    st r2[3] <- r1\n"
                             "    mov r1 <- r2");
            break;
        case 5: //IO.out_int
            s = stringstream("    ld r2 <- fp[2]\n"
                             "    ld r1 <- r2[3]\n"
                             "    syscall IO.out_int\n"
                             "    mov r1 <- r0");
            break;
        case 6: //IO.out_string
            s = stringstream("    ld r2 <- fp[2]\n"
                             "    ld r1 <- r2[3]\n"
                             "    syscall IO.out_string\n"
                             "    mov r1 <- r0");
            break;
        case 7: //String.concat
            s = stringstream("    push fp\n"
                             "    push r0\n"
                             "    la r2 <- String..new\n"
                             "    call r2\n"
                             "    pop r0\n"
                             "    pop fp\n"
                             "    mov r3 <- r1\n"
                             "    ld r2 <- fp[2]\n"
                             "    ld r2 <- r2[3]\n"
                             "    ld r1 <- r0[3]\n"
                             "    syscall String.concat\n"
                             "    st r3[3] <- r1\n"
                             "    mov r1 <- r3");
            break;
        case 8: //String.length
            s = stringstream("    push fp\n"
                             "    push r0\n"
                             "    la r2 <- Int..new\n"
                             "    call r2\n"
                             "    pop r0\n"
                             "    pop fp\n"
                             "    mov r2 <- r1\n"
                             "    ld r1 <- r0[3]\n"
                             "    syscall String.length\n"
                             "    st r2[3] <- r1\n"
                             "    mov r1 <- r2");
            break;
        case 9: //String.substr
            s = stringstream("    push fp\n"
                             "    push r0\n"
                             "    la r2 <- String..new\n"
                             "    call r2\n"
                             "    pop r0\n"
                             "    pop fp\n"
                             "    mov r3 <- r1\n"
                             "    ld r2 <- fp[2]\n"
                             "    ld r2 <- r2[3]\n"
                             "    ld r1 <- fp[3]\n"
                             "    ld r1 <- r1[3]\n"
                             "    ld r0 <- r0[3]\n"
                             "    syscall String.substr\n"
                             "    bnz r1 l3\n"
                             "    la r1 <- substr\n"
                             "    syscall IO.out_string\n"
                             "    syscall exit\n"
                             "l3:\n"
                             "    st r3[3] <- r1\n"
                             "    mov r1 <- r3");
            break;
        case 10:
            s = stringstream("eq_handler:             ;; helper function for =\n"
                             "                        mov fp <- sp\n"
                             "                        pop r0\n"
                             "                        push ra\n"
                             "                        ld r1 <- fp[3]\n"
                             "                        ld r2 <- fp[2]\n"
                             "                        beq r1 r2 eq_true\n"
                             "                        li r3 <- 0\n"
                             "                        beq r1 r3 eq_false\n"
                             "                        beq r2 r3 eq_false\n"
                             "                        ld r1 <- r1[0]\n"
                             "                        ld r2 <- r2[0]\n"
                             "                        ;; place the sum of the type tags in r1\n"
                             "                        add r1 <- r1 r2\n"
                             "                        li r2 <- 0\n"
                             "                        beq r1 r2 eq_bool\n"
                             "                        li r2 <- 2\n"
                             "                        beq r1 r2 eq_int\n"
                             "                        li r2 <- 6\n"
                             "                        beq r1 r2 eq_string\n"
                             "                        ;; otherwise, use pointer comparison\n"
                             "                        ld r1 <- fp[3]\n"
                             "                        ld r2 <- fp[2]\n"
                             "                        beq r1 r2 eq_true\n"
                             "eq_false:               ;; not equal\n"
                             "                        ;; new Bool\n"
                             "                        push fp\n"
                             "                        push r0\n"
                             "                        la r2 <- Bool..new\n"
                             "                        call r2\n"
                             "                        pop r0\n"
                             "                        pop fp\n"
                             "                        jmp eq_end\n"
                             "eq_true:                ;; equal\n"
                             "                        ;; new Bool\n"
                             "                        push fp\n"
                             "                        push r0\n"
                             "                        la r2 <- Bool..new\n"
                             "                        call r2\n"
                             "                        pop r0\n"
                             "                        pop fp\n"
                             "                        li r2 <- 1\n"
                             "                        st r1[3] <- r2\n"
                             "                        jmp eq_end\n"
                             "eq_bool:                ;; two Bools\n"
                             "eq_int:                 ;; two Ints\n"
                             "                        ld r1 <- fp[3]\n"
                             "                        ld r2 <- fp[2]\n"
                             "                        ld r1 <- r1[3]\n"
                             "                        ld r2 <- r2[3]\n"
                             "                        beq r1 r2 eq_true\n"
                             "                        jmp eq_false\n"
                             "eq_string:              ;; two Strings\n"
                             "                        ld r1 <- fp[3]\n"
                             "                        ld r2 <- fp[2]\n"
                             "                        ld r1 <- r1[3]\n"
                             "                        ld r2 <- r2[3]\n"
                             "                        ld r1 <- r1[0]\n"
                             "                        ld r2 <- r2[0]\n"
                             "                        beq r1 r2 eq_true\n"
                             "                        jmp eq_false\n"
                             "eq_end:                 pop ra\n"
                             "                        li r2 <- 2\n"
                             "                        add sp <- sp r2\n"
                             "                        return\n"
                             "                        ;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n"
                             "le_handler:             ;; helper function for <=\n"
                             "                        mov fp <- sp\n"
                             "                        pop r0\n"
                             "                        push ra\n"
                             "                        ld r1 <- fp[3]\n"
                             "                        ld r2 <- fp[2]\n"
                             "                        beq r1 r2 le_true\n"
                             "                        li r3 <- 0\n"
                             "                        beq r1 r3 le_false\n"
                             "                        beq r2 r3 le_false\n"
                             "                        ld r1 <- r1[0]\n"
                             "                        ld r2 <- r2[0]\n"
                             "                        ;; place the sum of the type tags in r1\n"
                             "                        add r1 <- r1 r2\n"
                             "                        li r2 <- 0\n"
                             "                        beq r1 r2 le_bool\n"
                             "                        li r2 <- 2\n"
                             "                        beq r1 r2 le_int\n"
                             "                        li r2 <- 6\n"
                             "                        beq r1 r2 le_string\n"
                             "                        ;; for non-primitives, equality is our only hope\n"
                             "                        ld r1 <- fp[3]\n"
                             "                        ld r2 <- fp[2]\n"
                             "                        beq r1 r2 le_true\n"
                             "le_false:               ;; not less-than-or-equal\n"
                             "                        ;; new Bool\n"
                             "                        push fp\n"
                             "                        push r0\n"
                             "                        la r2 <- Bool..new\n"
                             "                        call r2\n"
                             "                        pop r0\n"
                             "                        pop fp\n"
                             "                        jmp le_end\n"
                             "le_true:                ;; less-than-or-equal\n"
                             "                        ;; new Bool\n"
                             "                        push fp\n"
                             "                        push r0\n"
                             "                        la r2 <- Bool..new\n"
                             "                        call r2\n"
                             "                        pop r0\n"
                             "                        pop fp\n"
                             "                        li r2 <- 1\n"
                             "                        st r1[3] <- r2\n"
                             "                        jmp le_end\n"
                             "le_bool:                ;; two Bools\n"
                             "le_int:                 ;; two Ints\n"
                             "                        ld r1 <- fp[3]\n"
                             "                        ld r2 <- fp[2]\n"
                             "                        ld r1 <- r1[3]\n"
                             "                        ld r2 <- r2[3]\n"
                             "                        ble r1 r2 le_true\n"
                             "                        jmp le_false\n"
                             "le_string:              ;; two Strings\n"
                             "                        ld r1 <- fp[3]\n"
                             "                        ld r2 <- fp[2]\n"
                             "                        ld r1 <- r1[3]\n"
                             "                        ld r2 <- r2[3]\n"
                             "                        ld r1 <- r1[0]\n"
                             "                        ld r2 <- r2[0]\n"
                             "                        ble r1 r2 le_true\n"
                             "                        jmp le_false\n"
                             "le_end:                 pop ra\n"
                             "                        li r2 <- 2\n"
                             "                        add sp <- sp r2\n"
                             "                        return\n"
                             "                        ;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n"
                             "lt_handler:             ;; helper function for <\n"
                             "                        mov fp <- sp\n"
                             "                        pop r0\n"
                             "                        push ra\n"
                             "                        ld r1 <- fp[3]\n"
                             "                        ld r2 <- fp[2]\n"
                             "                        li r3 <- 0\n"
                             "                        beq r1 r3 lt_false\n"
                             "                        beq r2 r3 lt_false\n"
                             "                        ld r1 <- r1[0]\n"
                             "                        ld r2 <- r2[0]\n"
                             "                        ;; place the sum of the type tags in r1\n"
                             "                        add r1 <- r1 r2\n"
                             "                        li r2 <- 0\n"
                             "                        beq r1 r2 lt_bool\n"
                             "                        li r2 <- 2\n"
                             "                        beq r1 r2 lt_int\n"
                             "                        li r2 <- 6\n"
                             "                        beq r1 r2 lt_string\n"
                             "                        ;; for non-primitives, < is always false\n"
                             "lt_false:               ;; not less than\n"
                             "                        ;; new Bool\n"
                             "                        push fp\n"
                             "                        push r0\n"
                             "                        la r2 <- Bool..new\n"
                             "                        call r2\n"
                             "                        pop r0\n"
                             "                        pop fp\n"
                             "                        jmp lt_end\n"
                             "lt_true:                ;; less than\n"
                             "                        ;; new Bool\n"
                             "                        push fp\n"
                             "                        push r0\n"
                             "                        la r2 <- Bool..new\n"
                             "                        call r2\n"
                             "                        pop r0\n"
                             "                        pop fp\n"
                             "                        li r2 <- 1\n"
                             "                        st r1[3] <- r2\n"
                             "                        jmp lt_end\n"
                             "lt_bool:                ;; two Bools\n"
                             "lt_int:                 ;; two Ints\n"
                             "                        ld r1 <- fp[3]\n"
                             "                        ld r2 <- fp[2]\n"
                             "                        ld r1 <- r1[3]\n"
                             "                        ld r2 <- r2[3]\n"
                             "                        blt r1 r2 lt_true\n"
                             "                        jmp lt_false\n"
                             "lt_string:              ;; two Strings\n"
                             "                        ld r1 <- fp[3]\n"
                             "                        ld r2 <- fp[2]\n"
                             "                        ld r1 <- r1[3]\n"
                             "                        ld r2 <- r2[3]\n"
                             "                        ld r1 <- r1[0]\n"
                             "                        ld r2 <- r2[0]\n"
                             "                        blt r1 r2 lt_true\n"
                             "                        jmp lt_false\n"
                             "lt_end:                 pop ra\n"
                             "                        li r2 <- 2\n"
                             "                        add sp <- sp r2\n"
                             "                        return");
            break;
    }
}
void _program::pushBackHardcodedMethods() {
    //parallel vectors
    vector<string> klasses{"Object", "Object", "Object", "IO", "IO", "IO", "IO", "String", "String", "String"};
    vector<string> methods{"abort", "copy", "type_name", "in_int", "in_string", "out_int", "out_string", "concat", "length", "substr"};
    vector<int> maxStackRoom{1,1,1,1,1,2,2,2,1,3};
    stringstream s; string line;

    for(int i = 0; i < 10; ++i) {
        //push back method label
        code.push_back(klasses[i] + '.' + methods[i] + ':');
        calleeCallSequence(methods[i]); //use default value 1 for stackRoomForTemps
        //COMMENT
        code.push_back("\t;;method body begins;;;;;;;;;;;");
        setUpHardcodeStringStream(i, s);
        while(getline(s, line)) {
            code.push_back(line);
        }
        calleeReturnSequence(klasses[i], methods[i], maxStackRoom[i]);
    }
}

void _program::pushBackHardcodedHelpers() {
    //COMMENT
    code.push_back(";;start helper subroutines;;;;;;;;;;;;;;;;;;;;;;;;;;;");
    stringstream s; string line;
    setUpHardcodeStringStream(10, s);
    while(getline(s, line)) {
        code.push_back(line);
    }
    //COMMENT
    code.push_back(";;end helper subroutines;;;;;;;;;;;;;;;;;;;;;;;;;;;");
}

void _program::genMethods() {
    //TODO: implement DFS of class hierarchy starting at Object, each depth being taken care of according operator<
     //the above isn't needed at all but would help with debugging against the ref compiler
    //level order traversal of the hierarchy
//    string currentParent = "Object";
//    vector<string> klassPrintOrder{"Object"};
    //COMMENT
    code.push_back(";;start method generation;;;;;;;;;;");

    pushBackHardcodedMethods();

    for(auto x : classMapOrdered) {
        top = globalEnv->links.at({x.first, "class"});
        const string& klass = x.first;
        for(pair<methodRecord*, string> methodAndDefiner : implementationMapOrdered.at(klass)) {
            //only process if it is a defining class AND not one of the hardcoded classes
            if(klass != "Object" && klass != "IO" && klass != "String" && klass == methodAndDefiner.second) {
                currentMethod = methodAndDefiner.first->lexeme;
                currentClass = klass;
                code.push_back(klass + '.' + methodAndDefiner.first->lexeme + ':');

                int stackRoomForTemps = max(methodAndDefiner.first->maxTemps, 1);
                calleeCallSequence(methodAndDefiner.first->lexeme, stackRoomForTemps);

                top = globalEnv->links.at({klass, "class"});
                int i = 0;
                for(objectRecord* curAttr : classMapOrdered.at(klass)) {
                    //COMMENT
                    code.push_back("\t;; self[" + to_string(firstAttributeOffset + i++) + "] holds field " + curAttr->lexeme + " (" + curAttr->type + ')');
                }
                list<_formal*>& formals = methodAndDefiner.first->treeNode->formalList;
                i = firstArgOffset + formals.size() - 1;
                for(_formal* formal : formals) {
                    //COMMENT
                    code.push_back("\t;; fp[" + to_string(i--) + "] holds argument " + formal->identifier.identifier + " (" + formal->typeIdentifier.identifier + ')');
                }
                //COMMENT
                code.push_back("\t;; method body begins");

                top = globalEnv->links.at({klass, "class"})->links.at({methodAndDefiner.first->lexeme, "method"});
//                methodAndDefiner.first->treeNode->body->codeGen();
                methodAndDefiner.first->treeNode->codeGen();
                //COMMENT
                code.push_back("\t;;method body ends;;;;;;;;;;;");
                calleeReturnSequence(klass, methodAndDefiner.first->lexeme, stackRoomForTemps + methodAndDefiner.first->treeNode->formalList.size()); //TODO: calculate stackRoomForTemps
            }
        }
    }
    //COMMENT
    code.push_back(";;end method generation;;;;;;;;;;");

}

void _program::tackOnGlobalStringConstants() {
    //COMMENT
    code.push_back(";;global string constants;;;;;;;;;;;;;");
    code.insert(code.end(), globalStringConstants.begin(), globalStringConstants.end());
}


/**
 * print vtables, constructors, methods, and start label
 * @param instructions
 */
void _program::codeGen() {
    fillInClassAttributeNum();
    populateClassMapOrdered();
    populateImplementationMapOrdered();

    genVTablesAndGlobalStringConstantsClassNames();
    genConstructors();
    genMethods();
    tackOnGlobalStringConstants(); //this needs to be after the other codeGen
    pushBackHardcodedHelpers();
    genStart();

    actuallyPrint();
}

void _method::codeGen() {
    body->codeGen();
}

/**
* an integer node always makes an Integer assembly object. As noted elsewhere,
* 0,1,2 are for classTag, objectSize, and Klass..vtable. Then attributes start.
* For an integer assembly object, the only "attribute" is the raw int value.
* So it is at position 3.
*/
void _integer::codeGen() {
    if(rootExpression && !_method::letInitializer) callerConstructorCallAndReturnSequence("Int");
    li(temp_reg, value);
    st(acc_reg, firstAttributeOffset, temp_reg);
}

void _bool::codeGen() {
    if(rootExpression && !_method::letInitializer) callerConstructorCallAndReturnSequence("Bool");
    if(value) {
        li(temp_reg, value);
        st(acc_reg, firstAttributeOffset, temp_reg);
    } //default initialized to 0 (false) so don't do anything if false

}

/**
 * TODO: stop using vector for globalStringConstants so that we don't use multiple
 *      /different locations for copies of a constant that happens to show up more than once
 *
 */
void _string::codeGen() {
    if(rootExpression && !_method::letInitializer) callerConstructorCallAndReturnSequence("String");

    string constLabel = "string" + to_string(_string::stringCounter++);
    globalStringConstants.push_back(constLabel + ':');
    globalStringConstants.push_back("\tconstant \"" + value + '\"');
    //COMMENT-- this might break with \n and \t characters
    code.push_back("\t;; " + constLabel + " holds \"" + value + '\"');
    la(temp_reg, constLabel);
    st(acc_reg, firstAttributeOffset, temp_reg);
}

void _arith::codeGen() {
//    isLHS = true;
    left->codeGen();
    st(fp, 0, acc_reg);
//    isLHS = false;
    right->codeGen();
    ld(temp_reg, fp, 0);
    if(op == "plus") {
        add(acc_reg, temp_reg, acc_reg);
    }
    else if(op == "minus") {
        sub(acc_reg, temp_reg, acc_reg);
    }
    else if(op == "times") {
        mul(acc_reg, temp_reg, acc_reg);
    }
    else {//op == divides
        div(acc_reg, temp_reg, acc_reg);
    }
    st(fp, 0, acc_reg);

    //_arith nodes always return an Int    if(!rootExpression) ld(acc_reg, acc_reg, firstAttributeOffset);
    currentClass = "Int";
    currentMethod = ".new";
    callerConstructorCallAndReturnSequence(currentClass);

    ld(temp_reg, fp, 0);
    st(acc_reg, firstAttributeOffset, temp_reg);

    if(!rootExpression) ld(acc_reg, acc_reg, firstAttributeOffset);
}

int _method::fpOffset;
void _identifier::codeGen() {
    //COMMENT
    code.push_back("\t;; " + identifier.identifier);
    objectRecord* id = (objectRecord*)top->getObject(identifier.identifier);

    int offsetWRTfirstAttribute = 0;
    if(id->kind == "attribute") {//has an offset
        list<objectRecord*>& listtt = classMapOrdered.at(currentClass);
        for(objectRecord* current : listtt) {
            if(current == id) {
                break;
            }
            offsetWRTfirstAttribute++;
        }
        ld(acc_reg, self_reg, firstAttributeOffset + offsetWRTfirstAttribute);
        if(!rootExpression) ld(acc_reg, acc_reg, firstAttributeOffset);//TODO: DOES THIS GO HERE
    }
    //the only local variables in a methodEnv are the method parameters
    else if( (top->metaInfo.kind == "method") && (id->kind == "local") ) { //method parameter. need to refer using fp
        pair<methodRecord*, string> methodRec = implementationMap.at(top->C).at(top->metaInfo.identifier);
        list<_formal*>::iterator formalsIt = methodRec.first->treeNode->formalList.begin();
        int i = 0;
        while(i < methodRec.first->treeNode->formalList.size()) {
            if((*(formalsIt++))->identifier.identifier == id->lexeme) {
                break;
            }
            ++i;

        }
        ld(acc_reg, fp, (firstArgOffset + methodRec.first->treeNode->formalList.size() - 1) - i);
        //TODO does this go here?
        if(!rootExpression) ld(acc_reg, acc_reg, firstAttributeOffset);
    }
    else if(top->metaInfo.kind == "let" || top->metaInfo.kind == "case") { //should be all other possible variables (locals)
        ld(acc_reg, fp, - id->fpOffset);
        if(!rootExpression)ld(acc_reg, acc_reg, firstAttributeOffset);
    }
    else {
        code.push_back("\t;; did nothing with _identifier");
    }
}

void _selfDispatch::codeGen() {

    //COMMENT
    code.push_back("\t;;" + method.identifier + "(...)");
    push(self_reg);
    push(fp);

    for(_expr* arg : args) {
        arg->rootExpression = true;//not sure if ill still use this
        arg->codeGen();
        push(acc_reg);
    }

    push(self_reg);

    //COMMENT
    code.push_back("\t;;obtain vtable for self object of type " + top->C);
    ld(temp_reg, self_reg, vtablePointerOffset);

    //todo: include this info on the method record so we don't have to traverse to find its position
    int methodOffset = firstMethodOffset;
    list<pair<methodRecord*, string>>& methods = implementationMapOrdered.at(top->C);
    for(pair<methodRecord*, string> methodPair : methods) {
        if(methodPair.first->lexeme == method.identifier) break;
        ++methodOffset;
    }
    //COMMENT
    code.push_back("\t;;look up " + method.identifier + "() at offset " + to_string(methodOffset) + " in vtable");
    ld(temp_reg, temp_reg, methodOffset);
    call(temp_reg);
    pop(fp);
    pop(r0);
}
vector<string> _dispatch::callChain;
int _dynamicDispatch::labelCounter = 0;
void _dynamicDispatch::codeGen() {
    bool chainWasEmpty = false;

    //COMMENT

    if(callChain.empty()) { //will happen once on most nested recursion
        caller->rootExpression = true;
        _dynamicDispatch* curDispatch = this;

        while(curDispatch->caller->isDynamicDispatch) {
            callChain.push_back(curDispatch->method.identifier);
            curDispatch = (_dynamicDispatch*)curDispatch->caller;
        }
        callChain.push_back(curDispatch->method.identifier);
        reverse(callChain.begin(), callChain.end());
    }
    if(!callChain.empty()) {
        //STILL COMMENT;
        string comment = "(rootExpr)";
        for(string dispatch : callChain) {
            comment += '.' + dispatch + "(...)";
        }
        code.push_back("\t;; " + comment);
        callChain.erase(--callChain.end());
    }
    push(self_reg);
    push(fp);

    for(_expr* arg : args) {
        arg->rootExpression = true;//not sure if ill still use this
        arg->codeGen();
        push(acc_reg);
    }

    caller->codeGen();

    code.push_back("\t;; is_void check");

    string label = "dynDisp_l" + to_string(_dynamicDispatch::labelCounter++);
    bnz(acc_reg, label);
    la(acc_reg, is_voidLabel);
    syscall("IO.out_string");
    syscall("exit");

    code.push_back(label + ':');
    push(acc_reg);

    //COMMENT
    code.push_back("\t;; obtain vtable from object in " + acc_reg + " with static type " + caller->exprType);
    ld(temp_reg, acc_reg, vtablePointerOffset);

    int offset = firstMethodOffset;
    for(pair<methodRecord*, string> methodPair : implementationMapOrdered.at(top->C)) {
        if(methodPair.first->lexeme == method.identifier) break;
        offset++;
    }
    //COMMENT
    code.push_back("\t;; look up " + method.identifier + "() at offset " + to_string(offset) + " in vtable");

    ld(temp_reg, temp_reg, offset);
    call(temp_reg);
    pop(fp);
    pop(self_reg);
}

void _block::codeGen() {
    for(_expr* e : body) {
        e->codeGen();
    }
}

void _new::codeGen() {
    if(!isInitializer) { //do nothing if this is an initializer expression
        if(identifier.identifier == "SELF_TYPE") callerConstructorCallAndReturnSequence(lookUpSelfType(top));
        else {
            callerConstructorCallAndReturnSequence(identifier.identifier);
        }
    }
}

void _let::codeGen() {
    if (top->metaInfo.kind == "method")
        _method::fpOffset = 0;

    int fpOffsetOld = _method::fpOffset;
    for (_letBinding *binding : bindings) {
        binding->rootExpression = true;
        //COMMENT
        code.push_back("\t;; fp[" + to_string(-_method::fpOffset) + "] holds local " + binding->identifier.identifier + " (" + binding->typeIdentifier.identifier + ')');
        binding->codeGen(); //codegen must be in right environment, which EXCLUDES vars introduced in this let
        binding->fpOffset = _method::fpOffset++;
    }

    top = top->links.at({letKey.identifier, "let"});
    for(_letBinding *binding : bindings) {
        ((objectRecord*)top->symTable.at({binding->identifier.identifier, "local"}))->fpOffset = fpOffsetOld++;
    }
    body->rootExpression = true;
    body->codeGen();
    top = top->previous;
}

void _letBindingNoInit::codeGen() {
    callerConstructorCallAndReturnSequence(typeIdentifier.identifier);
    st(fp, -_method::fpOffset, acc_reg);
}

bool _method::letInitializer = false;
void _letBindingInit::codeGen() {
    //TODO: see when to conditionally apply this.
    //TODO: figured it out. don't need to call an assembly constructor if the initializer expression is an
    //_identifier, since it doesn't need to be reallocated
    if(!init->isIdentifierExpr)callerConstructorCallAndReturnSequence(typeIdentifier.identifier);
    init->rootExpression = true;
    _method::letInitializer = true;
    init->codeGen();
    _method::letInitializer = false;
    st(fp, -_method::fpOffset, acc_reg);
}

int _if::labelCounter = 0;
void _if::codeGen() {
    predicate->rootExpression = true;
    predicate->codeGen();

    string trueLabel = "if_l" + to_string(labelCounter++);
    string falseLabel = "if_l" + to_string(labelCounter++);
    string endLabel = "fi_l" + to_string(labelCounter++);

    ld(acc_reg, acc_reg, firstAttributeOffset);
    bnz(acc_reg, trueLabel);

    code.push_back(falseLabel + ':');
    //COMMENT
    code.push_back("\t;; false branch");
    elseExpr->rootExpression = true;
    elseExpr->codeGen();
    jmp(endLabel);

    code.push_back(trueLabel + ':');
    //COMMENT
    code.push_back("\t;; true branch");
    thenExpr->rootExpression = true;
    thenExpr->codeGen();
    //no jmp here, fall through
    code.push_back(endLabel + ':');
}

int _isvoid::labelCounter = 1;
void _isvoid::codeGen() {
    //COMMENT
    code.push_back("\t;; start isvoid expression by evaluating argument");

    expr->rootExpression = true;
    expr->codeGen();

    const string trueLabel = "isvoid_l" + to_string(labelCounter++);
    const string falseLabel = "isvoid_l" + to_string(labelCounter++);
    const string endLabel = "isvoid_end_l" + to_string(labelCounter++);

    bz(acc_reg, trueLabel);

    code.push_back(falseLabel + ':');
    //COMMENT
    code.push_back("\t;; false branch of isvoid");
    callerConstructorCallAndReturnSequence("Bool");
    jmp(endLabel);

    code.push_back(trueLabel + ':');
    //COMMENT
    code.push_back("\t;; true branch of isvoid");
    callerConstructorCallAndReturnSequence("Bool");
    //initialize to true
    li(temp_reg, 1);
    st(acc_reg, firstAttributeOffset, temp_reg);

    code.push_back(endLabel + ':');
}

void _relational::codeGen() {
    push(self_reg);
    push(fp);
    //we treat < = <= as function calls in assembly because they are, but we don't ever need
    //stack space for temporaries because it is inherently 2 in a binary operation
    left->rootExpression = left->isInitializer = true;
    left->codeGen();
    push(acc_reg);

    right->rootExpression = right->isInitializer = true;
    right->codeGen();
    push(acc_reg);

    push(self_reg);
    if(op == "lt") call("lt_handler"); //TODO replace with a const strings that look in a map
    if(op == "le") call("le_handler"); //TODO replace with a const strings that look in a map
    if(op == "eq") call("eq_handler"); //TODO replace with a const strings that look in a map
    pop(fp);
    pop(self_reg);


}