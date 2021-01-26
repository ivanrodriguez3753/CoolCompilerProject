#ifndef COOLCOMPILERPROJECTALL_CODEGEN_H
#define COOLCOMPILERPROJECTALL_CODEGEN_H

class methodRecord;
class objectRecord;

void li(string r, int i);
void li(string r, string label);
void la(string r, string regOrLabel);
void st(string r_base, int offset, string r_value);
void call(string r);
void push(string r);
void pop(string r);
void mov(string r_dest, string r_source);
void syscall(string syscall);
void sub(string r_dest, string r_subtractFrom, string r_subtractThis);
void add(string r_dest, string r_addTo, string r_addThis);
void ld(string r_dest, string r_base, int offset);
void mul(string r_dest, string r_addTo, string r_addThis);
void div(string r_dest, string r_addTo, string r_addThis);
void alloc(string r_dest, string r_numWords);


void callerCallAndReturnSequence(string klass, string subroutine);

extern const string new_frame;
extern const string rreturn;
extern const string newSuffix;
extern const string vTableSuffix;


extern const int classTagOffset;
extern const int objectSizeOffset;
extern const int vtablePointerOffset;
extern const int firstAttributeOffset;

extern const int firstArgOffset;

extern map<string, list<objectRecord*>> classMapOrdered;
extern map<string, list<pair<methodRecord*, string>>> implementationMapOrdered;

/**
 * Labels, instructions, and comments. Each take up a line
 */
extern vector<string> code;
/**
 * entries alternate between label and its respective constant. Example with 4 entries:
 * string1:
 *      constant "Bool"
 * string2:
 *      constant "IO"
 *
 * where Bool..vtable has a pointer to string1 and IO..vtable has a pointer to string2
 */
extern vector<string> globalStringConstants;
extern const string emptyStringLabel;
extern const string abortLabel;
extern const string substrLabel;

/**
 *  maps KlassName.methodName to assembly code needed for that method
 */
extern map<string, vector<string>> hardcodedStarterMethods;

extern const string r0,r1,r2,r3,r4,r5,r7,sp,fp,ra;
extern const string& self_reg;
extern const string& acc_reg;
extern const string& temp_reg;

extern string currentClass;
extern string currentMethod;

void populateClassMapOrdered();

class Register {
protected:
    int offset = 0;
public:
    Register(int o) : offset{o} {}
};

class FP : public Register {
public:
    FP(int o) : Register{o} {}

};

class SP : public Register {
    SP(int o) : Register{o} {}
};

class R : public Register {
    R(int o) : Register{o} {}
};



#endif //COOLCOMPILERPROJECTALL_CODEGEN_H
