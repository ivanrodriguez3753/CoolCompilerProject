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
    /**
     * Used as a counter for when something was encountered, so that we can maintain the order in which things appear
     * in a COOL source file.
     */
    int encountered = 0;

    /**
    * Points to the current letCaseEnv
    */
    letCaseEnv* top = nullptr;

    /**
     * Points to the current classEnv
     */
    classEnv* currentClassEnv = nullptr;

    /**
     * Points to the current methodEnv
     */
    methodEnv* currentMethodEnv = nullptr;

    _program* bisonProduct;

    /**
     * Pointer to the root of the user-defined AST
     */
    _program* ast;

    /**
     * Pointer to the AST representing methods that come with COOL out of the box. Mirrors the tree built by bison
     */
    _program* internalsAst;

    /**
     * The root of the symbol table
     */
    globalEnv* env;

    /**
     * The three required LLVM API classes
     */
    llvm::LLVMContext* llvmContext = new llvm::LLVMContext();
    llvm::Module* llvmModule = new llvm::Module("theModuleID", *llvmContext);
    llvm::IRBuilder<>* llvmBuilder = new llvm::IRBuilder<>(*llvmContext);

    map<string, llvm::Type*> types;

    /**
     * Keeps track of current the current LLVM function in the LLVM we are in when codegen'ing. Typically set/restored
     * at the beginning/end of a dispatch expression or function definition
     */
    llvm::Function* cur_func;

    /**
     * Is inserted into alongside every call to llvmBuilder's SetInsertPoint. Since the default ordering of blocks
     * is the order in which they are declared, we need to keep track of when they were activated and order them
     * in the same order they appear in this vector.
     */
    vector<llvm::BasicBlock*> currentBlocks;

    /**
     * map a string to its enumerated @globalValue identifier in LLVM (names are @.str.x)
     */
    map<string, pair<int, llvm::Constant*>> strLits{
        {"", {0, llvmBuilder->CreateGlobalStringPtr(llvm::StringRef(""), ".str.0", 0, llvmModule)}}}; //empty string

    llvm::Value* percentdPtr = llvmBuilder->CreateGlobalStringPtr(llvm::StringRef("%d"), ".str.percentd", 0, llvmModule);
    /**
     * This compiler is implementing five (six if you consider static/dynamic dispatch different) runtime errors:
     * dispatch on void, case on void, executing a case statement without a matching branch, division by zero,
     * and substr out of range
     */
    enum RUNTIME_ERROR_CODES{
        STATIC_DISP_ON_VOID = 0, DYNAMIC_DISP_ON_VOID, CASE_ON_VOID, CASE_NO_MATCHING_BR, DIV_BY_ZERO, SUBSTR_OUT_OF_RANGE, SUBSTR_NEG_ARG
    };

    /**
     * Error messages for the runtime errors. Follow up with a call to printf that prints the line number of the
     * problematic expression, except for SUBSTR_OUT_OF_RANGE which is always reported as occurring on line 0.
     */
    map<int, llvm::Constant*> runtimeErrorStrings {
        {STATIC_DISP_ON_VOID, llvmBuilder->CreateGlobalStringPtr("RUNTIME ERROR: Static dispatch on void. Check line ", ".str.STATIC_DISP_ON_VOID", 0, llvmModule)},
        {DYNAMIC_DISP_ON_VOID, llvmBuilder->CreateGlobalStringPtr("RUNTIME ERROR: Dynamic dispatch on void. Check line ", ".str.DYNAMIC_DISP_ON_VOID", 0, llvmModule)},
        {CASE_ON_VOID, llvmBuilder->CreateGlobalStringPtr("RUNTIME ERROR: Case switchee is void. Check line ", ".str.CASE_ON_VOID", 0, llvmModule)},
        {CASE_NO_MATCHING_BR, llvmBuilder->CreateGlobalStringPtr("RUNTIME ERROR: No matching branch for case expression. Check line ", ".str.CASE_NO_MATCHING_BR", 0, llvmModule)},
        {DIV_BY_ZERO, llvmBuilder->CreateGlobalStringPtr("RUNTIME ERROR: Divide by zero. Check line ", ".str.DIV_BY_ZERO", 0, llvmModule)},
        {SUBSTR_OUT_OF_RANGE, llvmBuilder->CreateGlobalStringPtr("RUNTIME ERROR: Substring indices are out of range. Reporting error from internal function on line 0", ".str.SUBSTR_OUT_OF_RANGE", 0, llvmModule)},
        {SUBSTR_NEG_ARG, llvmBuilder->CreateGlobalStringPtr("RUNTIME ERROR: One or both arguments to substr has negative value. Reporting error from internal function on line 0", ".str.SUBSTR_NEG_ARG", 0, llvmModule)}
    };


    /**
     * Separate counter for letCase environments
     */
    int letCaseCounter = 0;

    /**
     * Maps a class to its attributes.
     */
    map<string, map<string, pair<objRec*, int>>> classMap;

    /**
     * Maps a class to its methods.
     * methodRec* has info about defining class, and int is the position. Don't change the position if the child overrides
     * a parent's method, only change the methodRec*
     */
    map<string, map<string, pair<methodRec*, int>>> implementationMap;

    /**
     * Inheritance graph of the program. Object is the root of the graph
     * <key, <parent, children> >
     */
    map<string, pair<string, set<string>>> inherGraph;

    /**
     * Since semantic errors do not cause an immediate halt, we keep a log of all semantic errors encountered.
     * We do not proceed to codegen unless this log is empty
     */
    vector<pair<int, string>> errorLog;

    ParserDriver();
    /**
     * builds all the non letCase environments and symbol tables, so it just traverses the top three levels of the tree
     */
    void buildEnvs();

    /**
     * Sets up a new scope for a let expression
     */
    letCaseEnv* buildLetEnv(_let* letNode);

    /**
     * Sets up new scopes for each branch of a case statement
     */
    vector<letCaseEnv*> buildCaseEnvs(_case* caseNode);

    /**
     * Hardcoded tree that represents COOL classes and methods that come out of the box
     */
    void buildInternalsAst();

    /**
     * Populates the class and implementation maps
     */
    void populateClassImplementationMaps();
        void populateMaps(string klass); //helper method

    /**
     * Driver call to the ast decorate methods
     */
    void decorateAST();

    /**
     * Follows the specification for serializing a COOL program to then check against the ref compiler
     */
    void printClassMap(ostream& os);
    void printImplementationMap(ostream& os);
    void printParentMap(ostream& os);

    /**
     * Driving call for all the ast codegen methods
     */
    void codegen();

    /**
     * Computes the least upper bound of a set of types using the <= relation induced by a class hierarchy
     */
    string computeLub(set<string> s);

    /**
     * Computes the truth value of T1 <= T2
     */
    bool conforms(string T1, string T2);


    int result;

    /**
     * Driving call to the parser generated by bison
     */
    int parse(const std::string& f);

    /**
     * Name of the file being parsed
     */
    std::string file;

    bool trace_parsing;

    /**
     * Method to handle init/end of the scanner generated by flex
     */
    void scan_begin();
    void scan_end();

    bool trace_scanning;

    /**
     * Token's location used by scanner
     */
    yy::location location;



    friend class _node;
private:
    ///////////////LLVM API methods////////////////////////////////
    /**
     * Declares external functions like malloc, memcpy, strcmp, etc
     */
    void declareExterns();

    /**
     * Hardcoded LLVMString class and associated methods.
     */
    void gen_llvmStringTypeAndMethods();

    /**
     * Declares all structs and functions so that the API is aware of them for when we reference them later
     */
    void declaresStructsAndFuncs();

    /**
     * Generates the virtual function table for each type
     */
    void genVTables();

    /**
     * Defines the members of a structure.
     */
    void genStructDefs();

    /**
     * Generates the LLVM constructors for the three immutable types in COOL
     */
    void genBoolIntStringCtrs();

    /**
     * Generates the LLVM constructors for Object, IO, and user defined types
     */
    void genCtrs();

    /**
     * Generates methods that come with COOL out of the box
     */
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

    /**
     * The bread and butter of codegen. Generates user defined methods for every user defined class
     */
    void genUserMethods();

    /**
     * Generates the entry to the LLVM program. Instantiates the implicit instance of Main and calls Main.main()
     */
    void genLLVMmain();

};


#endif //PARSERDRIVER_H
