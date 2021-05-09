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

    /**
    * points to the current letCaseEnv
    */
    letCaseEnv* top = nullptr;
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

    /**
     * Keeps track of current the current LLVM function in the LLVM we are in when codegen'ing. Typically set/restored
     * at the beginning/end of a dispatch expression or function definition
     */
    llvm::Function* cur_func;

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

    vector<pair<int, string>> errorLog;

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
    void decorateAST();

    void printClassMap(ostream& os);
    void printImplementationMap(ostream& os);
    void printParentMap(ostream& os);

    void codegen();

    string computeLub(set<string> s);
    bool conforms(string T1, string T2);


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
