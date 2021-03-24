/*
 * This file provides control mechanisms for the parser, which uses the lexer. Follows the tutorial for the
 * complete C++ parser in Chapter10 of the bison documentation
 */

#ifndef PARSERDRIVER_H
#define PARSERDRIVER_H


#include <map>
#include <string>
#include <set>
#include "ast.h"
#include "parser.hpp"
#include "environment.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"



//give Flex the prototype of yylex we want...
#define YY_DECL yy::parser::symbol_type yylex(ParserDriver& drv)
//...and declare it for the parser's sake
YY_DECL;

using namespace std;

class ParserDriver {
public:
    int encountered = 0;

    classEnv* currentClassEnv = nullptr;
    methodEnv* currentMethodEnv = nullptr;

    _program* bisonProduct;

    _program* ast;
    _program* internalsAst;
    globalEnv* env;

    llvm::LLVMContext* llvmContext = new llvm::LLVMContext();
    llvm::Module* llvmModule = new llvm::Module("theModuleID", *llvmContext);
    llvm::IRBuilder<>* llvmBuilder = new llvm::IRBuilder<>(*llvmContext);
    map<string, llvm::Value*> llvmNamedValues;
    map<string, llvm::Type*> types;

    llvm::Function* cur_func;

    /**
     * map a string to its enumerated @globalValue identifier in LLVM (names are @.str.x)
     */
    map<string, pair<int, llvm::Constant*>> strLits;

    /**
     * points to the current letCaseEnv
     */
    letCaseEnv* top = nullptr;

    /**
     * This map keeps track of all the available local identifiers for a function, which is known at compile time
     * Name mangling will be (letCaseEnv.id) + id for the keys, and the value is a pointer to the result of alloca,
     * where alloca is called once for each local identifier at the beginning of the LLVM code for each user function.
     * Initialization for the identifier (i.e. calling its ..ctr) happens only in the block in which that identifier
     * is declared in the cool source code. For example,
     * if isTrue then
     *      let x : Int ...
     * else
     *      let x : Int ...
     * fi
     * We would alloca for each x above, at the beginning of the LLVM user function definition, but we
     * call Int..ctr only in the block in which that identifier is introduced. Something like the following:
     * %let1.x = alloca %Int_c
     * %let2.x = alloca %Int_c
     *
     * %boolCheck = some boolean based on isTrue above
     * br i1 %boolCheck label %truePath, label %falsePath
     * truePath:
     *      call Int..ctr and pass in %let1.x
     *      ...do stuff
     *      br label %fi
     * falsePath:
     *      call Int..ctr and pass in %let2.x
     *      ...do stuff
     *      br label %fi
     * fi:
     *      ...whatever comes after the if statement
     */
    map<string, llvm::Value*> localsMap;
    int letCaseCounter = 0;

    map<string, map<string, pair<objRec*, int>>> classMap;

    /**
     * methodRec* has info about defining class, and int is the position. Don't change the position if the child overrides
     * a parent's method, only change the methodRec*
     */
    map<string, map<string, pair<methodRec*, int>>> implementationMap;

    /**
     * <key, <parent, children> >
     */
    map<string, pair<string, set<string>>> inherGraph;

    ParserDriver();
    /**
     * builds all the non letCase environments and symbol tables, so it just traverses the top three levels of the tree
     * TODO: can do this as part of the ast construction, maybe? but it would be bottom up
     */
    void buildEnvs();
    letCaseEnv* buildLetEnv(_let* letNode);
    vector<letCaseEnv*> buildCaseEnvs(_case* caseNode);
    void buildInternalsAst();
    void populateClassImplementationMaps();
    void populateMaps(string klass); //helper method

    void printClassMap(ostream& os);
    void printImplementationMap(ostream& os);
    void printParentMap(ostream& os);

    void codegen();

    string computeLub(set<string> s);

    int result;

    //main routine is calling the parser
    int parse(const std::string& f);


        //name of file being parsed
    std::string file;

    bool trace_parsing;

    //handle scanner
    void scan_begin();
    void scan_end();

    bool trace_scanning;

    //Token's location used by scanner
    yy::location location;



    friend class _node;
private:
    void declareExterns();
    void gen_llvmStringTypeAndMethods();
    void declaresStructsAndFuncs();
    void genVTables();
    void genStructDefs();
    void genBoolIntStringCtrs();
    void genCtrs();
    void genBasicClassMethods();
        void genIO_out_int();
        void genIO_out_string();
        void genIO_in_int();
        void genIO_in_string();
        void genObject_abort();
        void genObject_copy();
        void genObject_type_name();
        void genString_concat();
        void genString_length();
        void genString_substr();
    void genUserMethods();
    void genLLVMmain();

};


#endif //PARSERDRIVER_H
